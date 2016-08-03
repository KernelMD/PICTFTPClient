/* 
 * File:   tftp_client.c
 * Author: Titov Denis <kernelmd@yandex.ru>
 * 
 * Created on 01.08.2016, 19:00
 * 
 * This file is part of PicTFTPClient.
 * 
 * PicTFTPClient is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * PicTFTPClient is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with PicTFTPClient.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../include/tftp_client.h"
#include <string.h>

void tftpInit(TFTPConn* conn) {
    if (!conn)
        return;
    conn->oper = OP_NONE;
    memset(conn->uri, '\0', MAX_URI_LEN);
    conn->state = S_NOTCONNECTED;
    conn->timeout = DEFAULT_TIMEOUT;
    conn->tsize = 0;
    conn->seqnum = 0;
    memset(conn->block, 0, BLK_SIZE);
}

int tftpRead(TFTPConn* conn, uint8_t* dst, size_t len) {
    size_t sent;

    if (!conn || !dst)
        return -1;

    if (conn->oper != OP_READ && conn->oper != OP_NONE) {
        conn->state = S_EWRONGOP;
        return -1;
    } else if (conn->state != S_NOTCONNECTED &&
            conn->state != S_GOT && conn->state != S_ACK) {
        return -1;
    } else if (conn->seqnum == 0) { /* No information yet. */
        return 0;
    }

    sent = (len < BLK_SIZE) ? len : BLK_SIZE;
    memcpy(dst, conn->block, sent);
    /* Padded remains received. */
    if ((conn->tsize - (conn->seqnum * BLK_SIZE)) < BLK_SIZE)
        sent = conn->tsize - (conn->seqnum * BLK_SIZE);

    return sent;
}

int tftpWrite(TFTPConn* conn, const uint8_t* src, size_t len) {
    size_t sent;

    if (!conn || !src)
        return -1;

    if (conn->oper != OP_WRITE && conn->oper != OP_NONE) {
        conn->state = S_EWRONGOP;
        return -1;
    } else if (conn->state != S_NOTCONNECTED &&
            conn->state != S_GOT && conn->state != S_ACK) {
        return -1;
    }

    if (conn->state == S_ACK) { /* Transmission of previous block confirmed. */
        sent = (len < BLK_SIZE) ? len : BLK_SIZE;
        memcpy(conn->block, src, sent);

        if (sent < BLK_SIZE) { /* Pad with 0s, if neccessary. */
            memset(conn->block + sent, 0, BLK_SIZE - sent);
        }

        return sent;
    } else {
        return 0;
    }
}

int tftpRecieveData(TFTPConn* conn, const uint8_t* buf, size_t len) {
    size_t sent;
    if (!conn || !buf)
        return -1;

    if (len < BLK_SIZE - conn->blk_used)
        sent = len;
    else
        sent = BLK_SIZE - conn->blk_used;

    memcpy(conn->block + conn->blk_used, buf, sent);
    return sent;
}

int tftpTransmitData(TFTPConn* conn, uint8_t* buf, size_t len) {
    size_t sent;
    if (!conn || !buf)
        return -1;

    if (len < BLK_SIZE - conn->blk_used)
        sent = len;
    else
        sent = BLK_SIZE - conn->blk_used;

    memcpy(buf, conn->block + conn->blk_used, sent);
    return sent;
}