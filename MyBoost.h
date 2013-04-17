#pragma once

class Nothing {};

// member_if::type is the type if Cond is true, or a zero length array if Cond is false.
// This way, if cond is false, you can declare a member that is unusable and takes 0 bytes.
// Not standard: zero length arrays are "illegal", but work in GCC and Visual Studio (2012 at least).
template <int Cond, class T>
struct member_if
{
    typedef T type;
};

template<class T>
struct member_if<0, T>
{
    typedef Nothing type[0];
};
