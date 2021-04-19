/**
 * @file any.h
 * @author zjb
 * @brief 使用c++11实现通用类型any类
 * @date 2021-04-13
 */

#ifndef CONEX_ANY_H
#define CONEX_ANY_H
#include<memory>
#include<typeinfo>
namespace conex
{
class any
{
private:
    class holder;
    using holderPtr = std::shared_ptr<holder>;

    //用于类型擦除的数据基类
    class holder
    {
    public:
        virtual holderPtr clone() const = 0;
        virtual const std::type_info& type() const = 0;
        virtual ~holder()
        {}
    };
    //用于持有数据的类
    template<class value_type>
    class dataholder : public holder
    {
    private:
        using self = dataholder<value_type>;
    public:
        dataholder(const value_type& data)
            :data_(data)
        {}
        //禁止拷贝构造
        dataholder(const self&) = delete;
        template<class U>
        explicit dataholder(U&& data)
            :data_(std::forward<U>(data))
        {}

        holderPtr clone() const override
        {
            return static_cast<holderPtr>(new dataholder(data_));
        }
        const std::type_info& type() const override
        {
            return typeid(data_);
        }
        value_type data_;
    };

    holderPtr data_;

public:
    //友元函数any_cast
    template<class T>
    friend T& any_cast(const any& anyObj);

    template<class T>
    friend T& any_cast(any& anyObj);

    template<class T>
    friend T* any_cast(const any* anyPtr);

    template<class T>
    friend T* any_cast(any* anyPtr);

    //成员函数
    any()
        :data_(nullptr)
    {}
    any(const any& other)
        :data_(other.data_)
    {}
    template<class T>
    any(const T& data)
        :data_(new dataholder<T>(data))
    {}
    any(any&& other)
        :data_(std::move(other.data_)) //这个函数内部的调用过程？
    {
    }
    ~any() = default;

    any& operator=(const any& other)
    {
        any tmp(other);
        std::swap(*this,tmp);
        return *this;
    }
    any& operator=(any&& other) noexcept
    {
        if(this != &other)
        {
            data_ = std::move(other.data_);
        }
        return *this;
    }
    const std::type_info& type() const
    {
        return data_ == nullptr ? typeid(void) : data_->type();
    }
};

template<class T>
T& any_cast(any& anyObj)
{
    return static_cast<any::dataholder<T>*>(anyObj.data_);//基类指针转派生类指针
}

template<class T>
T& any_cast(const any& anyObj)
{
    return static_cast<any::dataholder<T>*>(anyObj.data_);
}

template<class T>
T* any_cast(any* anyPtr)
{
    return &(static_cast<any::dataholder<T>*>(anyPtr->data_.get())->data_);
}

template<class T>
T* any_cast(const any* anyPtr)
{
    return &(static_cast<any::dataholder<T>*>(anyPtr->data_.get())->data_);
}
}

#endif