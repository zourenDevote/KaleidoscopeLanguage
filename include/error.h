
#ifndef __KALE_ERROR__
#define __KALE_ERROR__

#define LOG_ERROR(str, lineInfo) printf("error: %d:%d-%d %s\n", InputFileList[lineInfo.FileIndex].c_str(), lineInfo.Row, lineInfo.Col, str); assert(false);

#endif // end if KALE_ERROR






