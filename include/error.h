
#ifndef KALE_ERROR
#define KALE_ERROR

#define LOG_ERROR(str, lineInfo, ...) printf("error: %d:%d-%d %s", InputFileList[lineInfo.FileIndex].c_str(), lineInfo.Row, lineInfo.Col, str, __VA_ARGS__);

#endif // end if KALE_ERROR






