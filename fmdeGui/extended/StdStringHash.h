#ifndef STDSTRINGHASH_H
#define STDSTRINGHASH_H

#include <string>

//хеш строки для qt
//пытался передавать по константной ссылке -- icc не компилит
unsigned int qHash(const std::string value);

#endif // STDSTRINGHASH_H
