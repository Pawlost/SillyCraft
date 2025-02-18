﻿#pragma once

#define FACE_SIDE_COUNT 6

UENUM()
enum class EFaceSide : uint8
{
	Front = 0,
	Back = 1,
	Right = 2,
	Left = 3,
	Bottom = 4,
	Top = 5
};
