// Copyright 2019 Team Sosweet. All Rights Reserved.

#pragma once

#include "ObjectMacros.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSaucewich, Error, Error)

#define _STRINGIZEX(x) #x

// 무엇이든 문자열로 만들어드립니다
#define STRINGIZE(x) _STRINGIZEX(x)

/**
 * Cond가 거짓이면 에러 로그를 출력합니다.
 * 아래와 같이 if문 안에서도 사용할 수 있습니다. Cond가 참일 경우 코드를 실행합니다.
 * if (GUARANTEE(Cond)) {...}
 */
#define GUARANTEE(Cond) _Guarantee((Cond), TEXT("Assertion failed: " #Cond " in " __FILE__ "(" STRINGIZE(__LINE__) "): " __FUNCTION__))

template <class T>
static bool _Guarantee(const bool bCond, T&& Text)
{
	if (!bCond) UE_LOG(LogSaucewich, Error, Text);
	return bCond;
}

UENUM(BlueprintType)
enum class EGunTraceHit : uint8
{
	None, Pawn, Other
};

