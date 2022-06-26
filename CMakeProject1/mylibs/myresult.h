#pragma once
#include "mylog.h"
#include "mythread.h"
#include "mymemory.h"
#include <string>
#include <vector>

#define DEFAULT_CODE 0
#define NORMAL_CODE 200
#define EXCEPTION_CODE 400
#define NULL_POINTER_EXCEPTION_CODE 401

#define DEFAULT_MESSAGE ""

/**
 * @brief 堆栈跟踪
 *
 */
using StackTrace = struct StackTrace
{
    StackTrace(std::string file, int line, int lineCount = 1);
    /* data */
    std::string _File;
    int _Line;
    int _LineCount;
    std::string _Statement;
    void Print();
};

template <class T>
class MyResult
{
public:
    enum Status
    {
        NORMAL,
        EXCEPTION
    };
    // constructors
    MyResult();
    explicit MyResult(T data);
    MyResult(MyResult<T>::Status status, int code, std::string const& message, T data);
    MyResult(Status status, int code, std::string const& message, T data, P<MyResult<T>> innerResult);
    MyResult(Status status, int code, std::string const& message, T data, P<MyResult<T>> innerResult, std::vector<StackTrace>& stackTrace);

    bool IsException();
    MyResult<T>* InnerResult();
    MyResult<T>* Print();
    MyResult<T>* PushToStack(StackTrace stackTrace);

    void Dispose();
    int Code() const;
    T Data();

    template <class T2>
    P<MyResult<T2>> As(T2 data);
    P<MyResult<T>> As(T data);

private:
    // 返回状态: NORMAL为正常返回, EXCEPTION为异常返回
    Status _Status;
    // 状态码, 可自定义
    int _Code;
    // 消息, 可自定义
    std::string _Message;
    // 返回值
    T _Data;
    // 内部返回或内部异常
    P<MyResult<T>> _InnerResult;
    // 调用堆栈
    std::vector<StackTrace> _StackTrace;

    void PrintOnce();

    void PrintRecursive();
};

template <class T>
MyResult<T>::MyResult()
{
    this->_Status = MyResult<T>::Status::NORMAL;
    this->_Code = DEFAULT_CODE;
    this->_Message = DEFAULT_MESSAGE;
    this->_Data = nullptr;
    this->_InnerResult = nullptr;
}

template <class T>
MyResult<T>::MyResult(T data)
{
    this->_Status = MyResult<T>::Status::NORMAL;
    this->_Code = NORMAL_CODE;
    this->_Message = DEFAULT_MESSAGE;
    this->_Data = data;
    this->_InnerResult = nullptr;
}

template <class T>
MyResult<T>::MyResult(
    MyResult<T>::Status status,
    int code,
    std::string const& message,
    T data)
{

    this->_Status = status;
    this->_Code = code;
    this->_Message = message;
    this->_Data = data;
    this->_InnerResult = nullptr;
}

template <class T>
MyResult<T>::MyResult(
    MyResult<T>::Status status,
    int code,
    std::string const& message,
    T data,
    P<MyResult<T>> innerResult)
{

    this->_Status = status;
    this->_Code = code;
    this->_Message = message;
    this->_Data = data;
    this->_InnerResult.reset(innerResult.release());
}

template <class T>
MyResult<T>::MyResult(
    Status status,
    int code,
    std::string const& message,
    T data,
    P<MyResult<T>> innerResult,
    std::vector<StackTrace>& stackTrace)
{
    this->_Status = status;
    this->_Code = code;
    this->_Message = message;
    this->_Data = data;
    this->_InnerResult.reset(innerResult.release());
    this->_StackTrace.assign(stackTrace.begin(), stackTrace.end());
}



template <class T>
bool MyResult<T>::IsException()
{
    return this->_Status == Status::EXCEPTION;
}

template <class T>
MyResult<T>* MyResult<T>::InnerResult()
{
    if (this->_InnerResult == nullptr)
    {
        return New<MyResult<T>>(
            Status::EXCEPTION,
            NULL_POINTER_EXCEPTION_CODE,
            "The Inner Result Is Null",
            nullptr,
            nullptr);
    }
    return this->_InnerResult;
}

template <class T>
void MyResult<T>::PrintOnce()
{
    PrintLn("Result %d: %s", this->_Code, this->_Message.c_str());
    std::vector<StackTrace> copiedStackTrace(this->_StackTrace);
    while (!copiedStackTrace.empty())
    {
        std::vector<StackTrace>::iterator popStackIterator = copiedStackTrace.begin();
        popStackIterator.base()->Print();
        copiedStackTrace.erase(popStackIterator);
    }
}

template <class T>
void MyResult<T>::PrintRecursive()
{
    if (this->_InnerResult == nullptr)
    {
        if (this->IsException())
        {
            Log::Error("ResultError", "An Exception In Result Occurred Caused By: ");
            Log::Error(
                std::to_string(this->_Code).c_str(),
                this->_Message);
        }
        this->PrintOnce();
        return;
    }
    this->_InnerResult->PrintRecursive();
    this->PrintOnce();
}

/**
 * @brief 打印调用栈
 *
 * @tparam T
 * @return MyResult<T>
 */
template <class T>
MyResult<T>* MyResult<T>::Print()
{
    this->PrintRecursive();
    return this;
}

/**
 * @brief 压入调用栈
 *
 * @tparam T
 * @param stackTrace
 * @return MyResult<T>*
 */
template <class T>
MyResult<T>* MyResult<T>::PushToStack(StackTrace stackTrace)
{
    this->_StackTrace.push_back(stackTrace);
    return this;
}

/**
 * @brief Dispose this result.
 *
 * @tparam T
 */
// template <class T>
// void MyResult<T>::Dispose()
// {
//     delete this;
// }

/**
 * @brief Get The Return Data And Dispose result.
 *  Note that this method just can be called ONCE ONLY.
 *
 * @tparam T
 * @return T
 */
template <class T>
T MyResult<T>::Data()
{
    T data = this->_Data;
    if (this->IsException())
    {
        this->Print();
    }
    // delete this;
    return data;
}

/**
 * @brief Get the code of this result.
 *
 * @tparam T
 * @return int
 */
template <class T>
int MyResult<T>::Code() const
{
    return _Code;
}

/**
 * @brief Change this result to another type(T2).
 *
 * @tparam T
 * @tparam T2
 * @param data
 * @return MyResult<T2>*
 */
template <class T>
template <class T2>
P<MyResult<T2>> MyResult<T>::As(T2 data)
{
    if (this->_InnerResult == nullptr)
    {
        return New<MyResult<T2>>(
            this->_Status == MyResult<T>::EXCEPTION ? MyResult<T2>::EXCEPTION : MyResult<T2>::NORMAL,
            this->_Code,
            this->_Message,
            data,
            P<MyResult<T2>>(nullptr),
            this->_StackTrace);
    }
    return New<MyResult<T2>>(
        this->_Status == MyResult<T>::EXCEPTION ? MyResult<T2>::EXCEPTION : MyResult<T2>::NORMAL,
        this->_Code,
        this->_Message,
        data,
        this->_InnerResult->As(data),
        this->_StackTrace);
}

template <class T>
P<MyResult<T>> MyResult<T>::As(T data)
{
    this->_Data = data;
    return P<MyResult<T>>(this);
}

///////////////////////////////////////////////////////////////////////////////////////

#define SOURCE_FILE(location) constexpr const char* SOURCE_FILE_LOCATION = location

#define THROW_EXCEPTION(result, line, returnData)                    \
    if (result->IsException())                                       \
    {                                                                \
        result->PushToStack(StackTrace(SOURCE_FILE_LOCATION, line)); \
        return result1->As(returnData);                              \
    }
