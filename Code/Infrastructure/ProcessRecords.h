/*JWHSSHVBSGPLHERE*/
/*
 * ProcessRecords.h
 *
 * Authors: James W Hegeman
 *          Syed Shabih Hasan
 *          Vivek B Sardeshmukh
 *
 */

#ifndef __PROCESS_RECORDS_H__
#define __PROCESS_RECORDS_H__

void ProcessRecords(unsigned int *distances, unsigned int *identifiers, unsigned int count, unsigned int query, unsigned int shutdown);
void memcpy_w(void *dest, const void *src, size_t count);

#endif
