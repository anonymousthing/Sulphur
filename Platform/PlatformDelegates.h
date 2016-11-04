#pragma once

//This file is not meant to be included directly from outside of Platform,
//but rather indirectly through Threading.h et al.

class AsyncProgress;

typedef void AsyncDelegate(AsyncProgress *progress, void *arg);
typedef void Method();
typedef int Delegate(void *arg);
