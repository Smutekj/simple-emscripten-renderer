/*!
\brief Contains the definition of the PixelFormat class used throughout the PowerVR Framework.
\file         PVRCore/texture/PixelFormat.h
\author       PowerVR by Imagination, Developer Technology Team
\copyright    Copyright (c) Imagination Technologies Limited.
*/
#pragma once

#include <cstdint>
#include <sstream>

namespace pvr {
/// <summary>Enumeration of all known Compressed pixel formats.</summary>
enum class CompressedPixelFormat
{
	PVRTCI_2bpp_RGB,
	PVRTCI_2bpp_RGBA,
	PVRTCI_4bpp_RGB,
	PVRTCI_4bpp_RGBA,
	PVRTCII_2bpp,
	PVRTCII_4bpp,
	ETC1,
	DXT1,
	DXT2,
	DXT3,
	DXT4,
	DXT5,

	// These formats are identical to some DXT formats.
	BC1 = DXT1,
	BC2 = DXT3,
	BC3 = DXT5,

	// These are currently unsupported:
	BC4,
	BC5,
	BC6,
	BC7,

	// These are supported
	UYVY,
	YUY2,
	BW1bpp,
	SharedExponentR9G9B9E5,
	RGBG8888,
	GRGB8888,
	ETC2_RGB,
	ETC2_RGBA,
	ETC2_RGB_A1,
	EAC_R11,
	EAC_RG11,

	ASTC_4x4,
	ASTC_5x4,
	ASTC_5x5,
	ASTC_6x5,
	ASTC_6x6,
	ASTC_8x5,
	ASTC_8x6,
	ASTC_8x8,
	ASTC_10x5,
	ASTC_10x6,
	ASTC_10x8,
	ASTC_10x10,
	ASTC_12x10,
	ASTC_12x12,

	ASTC_3x3x3,
	ASTC_4x3x3,
	ASTC_4x4x3,
	ASTC_4x4x4,
	ASTC_5x4x4,
	ASTC_5x5x4,
	ASTC_5x5x5,
	ASTC_6x5x5,
	ASTC_6x6x5,
	ASTC_6x6x6,

	BASISU_ETC1S,
	BASISU_UASTC,

	RGBM,
	RGBD,

	PVRTCI_HDR_6bpp,
	PVRTCI_HDR_8bpp,
	PVRTCII_HDR_6bpp,
	PVRTCII_HDR_8bpp,

	// The memory layout for 10 and 12 bit YUV formats that are packed into a WORD (16 bits) is denoted by MSB or LSB:
	// MSB denotes that the sample is stored in the most significant <N> bits
	// LSB denotes that the sample is stored in the least significant <N> bits
	// All YUV formats are little endian

	// Packed YUV formats
	VYUA10MSB_444,
	VYUA10LSB_444,
	VYUA12MSB_444,
	VYUA12LSB_444,
	UYV10A2_444,	// Y410
	UYVA16_444,		// Y416
	YUYV16_422,		// Y216
	UYVY16_422,
	YUYV10MSB_422,	// Y210
	YUYV10LSB_422,
	UYVY10MSB_422,
	UYVY10LSB_422,
	YUYV12MSB_422,
	YUYV12LSB_422,
	UYVY12MSB_422,
	UYVY12LSB_422,

	// 3 Plane (Planar) YUV formats
	YUV_3P_444 = 270,
	YUV10MSB_3P_444,
	YUV10LSB_3P_444,
	YUV12MSB_3P_444,
	YUV12LSB_3P_444,
	YUV16_3P_444,
	YUV_3P_422,
	YUV10MSB_3P_422,
	YUV10LSB_3P_422,
	YUV12MSB_3P_422,
	YUV12LSB_3P_422,
	YUV16_3P_422,
	YUV_3P_420,
	YUV10MSB_3P_420,
	YUV10LSB_3P_420,
	YUV12MSB_3P_420,
	YUV12LSB_3P_420,
	YUV16_3P_420,
	YVU_3P_420,

	// 2 Plane (Biplanar/semi-planar) YUV formats
	YUV_2P_422 = 480,	// P208
	YUV10MSB_2P_422,	// P210
	YUV10LSB_2P_422,
	YUV12MSB_2P_422,
	YUV12LSB_2P_422,
	YUV16_2P_422,		// P216
	YUV_2P_420,			// NV12
	YUV10MSB_2P_420,	// P010
	YUV10LSB_2P_420,
	YUV12MSB_2P_420,
	YUV12LSB_2P_420,
	YUV16_2P_420,		// P016
	YUV_2P_444,
	YVU_2P_444,
	YUV10MSB_2P_444,
	YUV10LSB_2P_444,
	YVU10MSB_2P_444,
	YVU10LSB_2P_444,
	YVU_2P_422,
	YVU10MSB_2P_422,
	YVU10LSB_2P_422,
	YVU_2P_420,			// NV21
	YVU10MSB_2P_420,
	YVU10LSB_2P_420,

	// IMG image compression (IMGIC)
	IMGIC_A8_8x8 = 700,
	IMGIC_A8_16x4,
	IMGIC_A8_32x2,
	IMGIC_R8_8x8,
	IMGIC_R8_16x4,
	IMGIC_R8_32x2,
	IMGIC_R16_8x8,
	IMGIC_R16_16x4,
	IMGIC_R16_32x2,
	IMGIC_R32_8x8,
	IMGIC_R32_16x4,
	IMGIC_R32_32x2,
	IMGIC_R8G8_8x8,
	IMGIC_R8G8_16x4,
	IMGIC_R8G8_32x2,
	IMGIC_R16G16_8x8,
	IMGIC_R16G16_16x4,
	IMGIC_R16G16_32x2,
	IMGIC_R32G32_8x8,
	IMGIC_R32G32_16x4,
	IMGIC_R32G32_32x2,
	IMGIC_R8G8B8_8x8,
	IMGIC_R8G8B8_16x4,
	IMGIC_R8G8B8_32x2,
	IMGIC_R16G16B16_8x8,
	IMGIC_R16G16B16_16x4,
	IMGIC_R16G16B16_32x2,
	IMGIC_R32G32B32_8x8,
	IMGIC_R32G32B32_16x4,
	IMGIC_R32G32B32_32x2,
	IMGIC_R8G8B8A8_8x8,
	IMGIC_R8G8B8A8_16x4,
	IMGIC_R8G8B8A8_32x2,
	IMGIC_R16G16B16A16_8x8,
	IMGIC_R16G16B16A16_16x4,
	IMGIC_R16G16B16A16_32x2,
	IMGIC_R32G32B32A32_8x8,
	IMGIC_R32G32B32A32_16x4,
	IMGIC_R32G32B32A32_32x2,
	IMGIC_A4R4G4B4_8x8,
	IMGIC_A4R4G4B4_16x4,
	IMGIC_A4R4G4B4_32x2,
	IMGIC_A8R3G3B2_8x8,
	IMGIC_A8R3G3B2_16x4,
	IMGIC_A8R3G3B2_32x2,
	IMGIC_A1R5G5B5_8x8,
	IMGIC_A1R5G5B5_16x4,
	IMGIC_A1R5G5B5_32x2,
	IMGIC_R5G5B5A1_8x8,
	IMGIC_R5G5B5A1_16x4,
	IMGIC_R5G5B5A1_32x2,
	IMGIC_R5G6B5_8x8,
	IMGIC_R5G6B5_16x4,
	IMGIC_R5G6B5_32x2,
	IMGIC_A2B10G10R10_8x8,
	IMGIC_A2B10G10R10_16x4,
	IMGIC_A2B10G10R10_32x2,
	IMGIC_R10B10G10A2_8x8,
	IMGIC_R10B10G10A2_16x4,
	IMGIC_R10B10G10A2_32x2,
	IMGIC_X8D24_8x8,
	IMGIC_X8D24_16x4,
	IMGIC_X8D24_32x2,
	IMGIC_S8D24_8x8,
	IMGIC_S8D24_16x4,
	IMGIC_S8D24_32x2,
	IMGIC_S8X24_8x8,
	IMGIC_S8X24_16x4,
	IMGIC_S8X24_32x2,
	IMGIC_D24S8_8x8,
	IMGIC_D24S8_16x4,
	IMGIC_D24S8_32x2,
	IMGIC_R9G9B9E5_8x8,
	IMGIC_R9G9B9E5_16x4,
	IMGIC_R9G9B9E5_32x2,
	IMGIC_R11G11B10_8x8,
	IMGIC_R11G11B10_16x4,
	IMGIC_R11G11B10_32x2,
	IMGIC_B10G11R11_8x8,
	IMGIC_B10G11R11_16x4,
	IMGIC_B10G11R11_32x2,
	IMGIC_B8G8R8A8_8x8,
	IMGIC_B8G8R8A8_16x4,
	IMGIC_B8G8R8A8_32x2,
	IMGIC_R8G8B8G8_8x8,
	IMGIC_R8G8B8G8_16x4,
	IMGIC_R8G8B8G8_32x2,
	IMGIC_G8R8G8B8_8x8,
	IMGIC_G8R8G8B8_16x4,
	IMGIC_G8R8G8B8_32x2,
	IMGIC_UYVY_8x8,
	IMGIC_UYVY_16x4,
	IMGIC_UYVY_32x2,
	IMGIC_YUYV_8x8,
	IMGIC_YUYV_16x4,
	IMGIC_YUYV_32x2,
	IMGIC_YUV_2P_444_8x8,
	IMGIC_YUV_2P_444_16x4,
	IMGIC_YUV_2P_444_32x2,
	IMGIC_YVU_2P_444_8x8,
	IMGIC_YVU_2P_444_16x4,
	IMGIC_YVU_2P_444_32x2,
	IMGIC_YVU10_2P_444_PK16_8x8,
	IMGIC_YVU10_2P_444_PK16_16x4,
	IMGIC_YVU10_2P_444_PK16_32x2,
	IMGIC_YUV_2P_422_8x8,
	IMGIC_YUV_2P_422_16x4,
	IMGIC_YUV_2P_422_32x2,
	IMGIC_YVU_2P_422_8x8,
	IMGIC_YVU_2P_422_16x4,
	IMGIC_YVU_2P_422_32x2,
	IMGIC_YVU10_2P_422_PK16_8x8,
	IMGIC_YVU10_2P_422_PK16_16x4,
	IMGIC_YVU10_2P_422_PK16_32x2,
	IMGIC_YUV_2P_420_8x8,
	IMGIC_YUV_2P_420_16x4,
	IMGIC_YUV_2P_420_32x2,
	IMGIC_YVU_2P_420_8x8,
	IMGIC_YVU_2P_420_16x4,
	IMGIC_YVU_2P_420_32x2,
	IMGIC_YVU10_2P_420_PK16_8x8,
	IMGIC_YVU10_2P_420_PK16_16x4,
	IMGIC_YVU10_2P_420_PK16_32x2,
	IMGIC_YUV_3P_444_8x8,
	IMGIC_YUV_3P_444_16x4,
	IMGIC_YUV_3P_444_32x2,
	IMGIC_YUV10_3P_444_PK16_8x8,
	IMGIC_YUV10_3P_444_PK16_16x4,
	IMGIC_YUV10_3P_444_PK16_32x2,
	IMGIC_YUV_3P_420_8x8,
	IMGIC_YUV_3P_420_16x4,
	IMGIC_YUV_3P_420_32x2,
	IMGIC_YVU_3P_420_8x8,
	IMGIC_YVU_3P_420_16x4,
	IMGIC_YVU_3P_420_32x2,

	// Invalid value
	NumCompressedPFs
};

inline std::string to_string(CompressedPixelFormat format)
{
	switch (format)
	{
	case CompressedPixelFormat::PVRTCI_2bpp_RGB: return "PVRTCI_2bpp_RGB";
	case CompressedPixelFormat::PVRTCI_2bpp_RGBA: return "PVRTCI_2bpp_RGBA";
	case CompressedPixelFormat::PVRTCI_4bpp_RGB: return "PVRTCI_4bpp_RGB";
	case CompressedPixelFormat::PVRTCI_4bpp_RGBA: return "PVRTCI_4bpp_RGBA";
	case CompressedPixelFormat::PVRTCII_2bpp: return "PVRTCII_2bpp";
	case CompressedPixelFormat::PVRTCII_4bpp: return "PVRTCII_4bpp";
	case CompressedPixelFormat::ETC1: return "ETC1";
	case CompressedPixelFormat::DXT1: return "DXT1";
	case CompressedPixelFormat::DXT2: return "DXT2";
	case CompressedPixelFormat::DXT3: return "DXT3";
	case CompressedPixelFormat::DXT4: return "DXT4";
	case CompressedPixelFormat::DXT5:
		return "DXT5";

		// These formats are identical to some DXT formats.
		// DXT1 = BC1
		// DXT3 = BC2
		// DXT5 = BC3

	case CompressedPixelFormat::BC4: return "BC4";
	case CompressedPixelFormat::BC5: return "BC5";
	case CompressedPixelFormat::BC6: return "BC6";
	case CompressedPixelFormat::BC7: return "BC7";

	case CompressedPixelFormat::UYVY: return "UYVY";
	case CompressedPixelFormat::YUY2: return "YUY2";
	case CompressedPixelFormat::BW1bpp: return "BW1bpp";
	case CompressedPixelFormat::SharedExponentR9G9B9E5: return "RGB9E5";
	case CompressedPixelFormat::RGBG8888: return "RGBG8888";
	case CompressedPixelFormat::GRGB8888: return "GRGB8888";
	case CompressedPixelFormat::ETC2_RGB: return "ETC2_RGB";
	case CompressedPixelFormat::ETC2_RGBA: return "ETC2_RGBA";
	case CompressedPixelFormat::ETC2_RGB_A1: return "ETC2_RGB_A1";
	case CompressedPixelFormat::EAC_R11: return "EAC_R11";
	case CompressedPixelFormat::EAC_RG11: return "EAC_RG11";

	case CompressedPixelFormat::ASTC_4x4: return "ASTC_4x4";
	case CompressedPixelFormat::ASTC_5x4: return "ASTC_5x4";
	case CompressedPixelFormat::ASTC_5x5: return "ASTC_5x5";
	case CompressedPixelFormat::ASTC_6x5: return "ASTC_6x5";
	case CompressedPixelFormat::ASTC_6x6: return "ASTC_6x6";
	case CompressedPixelFormat::ASTC_8x5: return "ASTC_8x5";
	case CompressedPixelFormat::ASTC_8x6: return "ASTC_8x6";
	case CompressedPixelFormat::ASTC_8x8: return "ASTC_8x8";
	case CompressedPixelFormat::ASTC_10x5: return "ASTC_10x5";
	case CompressedPixelFormat::ASTC_10x6: return "ASTC_10x6";
	case CompressedPixelFormat::ASTC_10x8: return "ASTC_10x8";
	case CompressedPixelFormat::ASTC_10x10: return "ASTC_10x10";
	case CompressedPixelFormat::ASTC_12x10: return "ASTC_12x10";
	case CompressedPixelFormat::ASTC_12x12: return "ASTC_12x12";

	case CompressedPixelFormat::ASTC_3x3x3: return "ASTC_3x3x3";
	case CompressedPixelFormat::ASTC_4x3x3: return "ASTC_4x3x3";
	case CompressedPixelFormat::ASTC_4x4x3: return "ASTC_4x4x3";
	case CompressedPixelFormat::ASTC_4x4x4: return "ASTC_4x4x4";
	case CompressedPixelFormat::ASTC_5x4x4: return "ASTC_5x4x4";
	case CompressedPixelFormat::ASTC_5x5x4: return "ASTC_5x5x4";
	case CompressedPixelFormat::ASTC_5x5x5: return "ASTC_5x5x5";
	case CompressedPixelFormat::ASTC_6x5x5: return "ASTC_6x5x5";
	case CompressedPixelFormat::ASTC_6x6x5: return "ASTC_6x6x5";
	case CompressedPixelFormat::ASTC_6x6x6: return "ASTC_6x6x6";

	case CompressedPixelFormat::BASISU_ETC1S: return "BASISU_ETC1S";
	case CompressedPixelFormat::BASISU_UASTC: return "BASISU_UASTC";

	case CompressedPixelFormat::RGBM: return "RGBM";
	case CompressedPixelFormat::RGBD: return "RGBD";

	case CompressedPixelFormat::PVRTCI_HDR_6bpp: return "PVRTCI_HDR_6bpp";
	case CompressedPixelFormat::PVRTCI_HDR_8bpp: return "PVRTCI_HDR_8bpp";
	case CompressedPixelFormat::PVRTCII_HDR_6bpp: return "PVRTCII_HDR_6bpp";
	case CompressedPixelFormat::PVRTCII_HDR_8bpp: return "PVRTCII_HDR_8bpp";

	// Packed YUV formats
	case CompressedPixelFormat::VYUA10MSB_444: return "VYUA10MSB_444";
	case CompressedPixelFormat::VYUA10LSB_444: return "VYUA10LSB_444";
	case CompressedPixelFormat::VYUA12MSB_444: return "VYUA12MSB_444";
	case CompressedPixelFormat::VYUA12LSB_444: return "VYUA12LSB_444";
	case CompressedPixelFormat::UYV10A2_444: return "UYV10A2_444";			// Y410
	case CompressedPixelFormat::UYVA16_444: return "UYVA16_444";			// Y416
	case CompressedPixelFormat::YUYV16_422: return "YUYV16_422";			// Y216
	case CompressedPixelFormat::UYVY16_422: return "UYVY16_422";
	case CompressedPixelFormat::YUYV10MSB_422: return "YUYV10MSB_422";		// Y210
	case CompressedPixelFormat::YUYV10LSB_422: return "YUYV10LSB_422";
	case CompressedPixelFormat::UYVY10MSB_422: return "UYVY10MSB_422";
	case CompressedPixelFormat::UYVY10LSB_422: return "UYVY10LSB_422";
	case CompressedPixelFormat::YUYV12MSB_422: return "YUYV12MSB_422";
	case CompressedPixelFormat::YUYV12LSB_422: return "YUYV12LSB_422";
	case CompressedPixelFormat::UYVY12MSB_422: return "UYVY12MSB_422";
	case CompressedPixelFormat::UYVY12LSB_422: return "UYVY12LSB_422";

	// 3 Plane (Planar) YUV formats
	case CompressedPixelFormat::YUV_3P_444: return "YUV_3P_444";
	case CompressedPixelFormat::YUV10MSB_3P_444: return "YUV10MSB_3P_444";
	case CompressedPixelFormat::YUV10LSB_3P_444: return "YUV10LSB_3P_444";
	case CompressedPixelFormat::YUV12MSB_3P_444: return "YUV12MSB_3P_444";
	case CompressedPixelFormat::YUV12LSB_3P_444: return "YUV12LSB_3P_444";
	case CompressedPixelFormat::YUV16_3P_444: return "YUV16_3P_444";
	case CompressedPixelFormat::YUV_3P_422: return "YUV_3P_422";
	case CompressedPixelFormat::YUV10MSB_3P_422: return "YUV10MSB_3P_422";
	case CompressedPixelFormat::YUV10LSB_3P_422: return "YUV10LSB_3P_422";
	case CompressedPixelFormat::YUV12MSB_3P_422: return "YUV12MSB_3P_422";
	case CompressedPixelFormat::YUV12LSB_3P_422: return "YUV12LSB_3P_422";
	case CompressedPixelFormat::YUV16_3P_422: return "YUV16_3P_422";
	case CompressedPixelFormat::YUV_3P_420: return "YUV_3P_420";
	case CompressedPixelFormat::YUV10MSB_3P_420: return "YUV10MSB_3P_420";
	case CompressedPixelFormat::YUV10LSB_3P_420: return "YUV10LSB_3P_420";
	case CompressedPixelFormat::YUV12MSB_3P_420: return "YUV12MSB_3P_420";
	case CompressedPixelFormat::YUV12LSB_3P_420: return "YUV12LSB_3P_420";
	case CompressedPixelFormat::YUV16_3P_420: return "YUV16_3P_420";
	case CompressedPixelFormat::YVU_3P_420: return "YVU_3P_420";

	// 2 Plane (Biplanar/semi-planar) YUV formats
	case CompressedPixelFormat::YUV_2P_422: return "YUV_2P_422";			// P208
	case CompressedPixelFormat::YUV10MSB_2P_422: return "YUV10MSB_2P_422";	// P210
	case CompressedPixelFormat::YUV10LSB_2P_422: return "YUV10LSB_2P_422";
	case CompressedPixelFormat::YUV12MSB_2P_422: return "YUV12MSB_2P_422";
	case CompressedPixelFormat::YUV12LSB_2P_422: return "YUV12LSB_2P_422";
	case CompressedPixelFormat::YUV16_2P_422: return "YUV16_2P_422";		// P216
	case CompressedPixelFormat::YUV_2P_420: return "YUV_2P_420";			// NV12
	case CompressedPixelFormat::YUV10MSB_2P_420: return "YUV10MSB_2P_420";	// P010
	case CompressedPixelFormat::YUV10LSB_2P_420: return "YUV10LSB_2P_420";
	case CompressedPixelFormat::YUV12MSB_2P_420: return "YUV12MSB_2P_420";
	case CompressedPixelFormat::YUV12LSB_2P_420: return "YUV12LSB_2P_420";
	case CompressedPixelFormat::YUV16_2P_420: return "YUV16_2P_420";		// P016
	case CompressedPixelFormat::YUV_2P_444: return "YUV_2P_444";
	case CompressedPixelFormat::YVU_2P_444: return "YVU_2P_444";
	case CompressedPixelFormat::YUV10MSB_2P_444: return "YUV10MSB_2P_444";
	case CompressedPixelFormat::YUV10LSB_2P_444: return "YUV10LSB_2P_444";
	case CompressedPixelFormat::YVU10MSB_2P_444: return "YVU10MSB_2P_444";
	case CompressedPixelFormat::YVU10LSB_2P_444: return "YVU10LSB_2P_444";
	case CompressedPixelFormat::YVU_2P_422: return "YVU_2P_422";
	case CompressedPixelFormat::YVU10MSB_2P_422: return "YVU10MSB_2P_422";
	case CompressedPixelFormat::YVU10LSB_2P_422: return "YVU10LSB_2P_422";
	case CompressedPixelFormat::YVU_2P_420: return "YVU_2P_420";			// NV21
	case CompressedPixelFormat::YVU10MSB_2P_420: return "YVU10MSB_2P_420";
	case CompressedPixelFormat::YVU10LSB_2P_420: return "YVU10LSB_2P_420";

	// IMG image compression (IMGIC)
	case CompressedPixelFormat::IMGIC_A8_8x8: return "IMGIC_A8_8x8";
	case CompressedPixelFormat::IMGIC_A8_16x4: return "IMGIC_A8_16x4";
	case CompressedPixelFormat::IMGIC_A8_32x2: return "IMGIC_A8_32x2";
	case CompressedPixelFormat::IMGIC_R8_8x8: return "IMGIC_R8_8x8";
	case CompressedPixelFormat::IMGIC_R8_16x4: return "IMGIC_R8_16x4";
	case CompressedPixelFormat::IMGIC_R8_32x2: return "IMGIC_R8_32x2";
	case CompressedPixelFormat::IMGIC_R16_8x8: return "IMGIC_R16_8x8";
	case CompressedPixelFormat::IMGIC_R16_16x4: return "IMGIC_R16_16x4";
	case CompressedPixelFormat::IMGIC_R16_32x2: return "IMGIC_R16_32x2";
	case CompressedPixelFormat::IMGIC_R32_8x8: return "IMGIC_R32_8x8";
	case CompressedPixelFormat::IMGIC_R32_16x4: return "IMGIC_R32_16x4";
	case CompressedPixelFormat::IMGIC_R32_32x2: return "IMGIC_R32_32x2";
	case CompressedPixelFormat::IMGIC_R8G8_8x8: return "IMGIC_R8G8_8x8";
	case CompressedPixelFormat::IMGIC_R8G8_16x4: return "IMGIC_R8G8_16x4";
	case CompressedPixelFormat::IMGIC_R8G8_32x2: return "IMGIC_R8G8_32x2";
	case CompressedPixelFormat::IMGIC_R16G16_8x8: return "IMGIC_R16G16_8x8";
	case CompressedPixelFormat::IMGIC_R16G16_16x4: return "IMGIC_R16G16_16x4";
	case CompressedPixelFormat::IMGIC_R16G16_32x2: return "IMGIC_R16G16_32x2";
	case CompressedPixelFormat::IMGIC_R32G32_8x8: return "IMGIC_R32G32_8x8";
	case CompressedPixelFormat::IMGIC_R32G32_16x4: return "IMGIC_R32G32_16x4";
	case CompressedPixelFormat::IMGIC_R32G32_32x2: return "IMGIC_R32G32_32x2";
	case CompressedPixelFormat::IMGIC_R8G8B8_8x8: return "IMGIC_R8G8B8_8x8";
	case CompressedPixelFormat::IMGIC_R8G8B8_16x4: return "IMGIC_R8G8B8_16x4";
	case CompressedPixelFormat::IMGIC_R8G8B8_32x2: return "IMGIC_R8G8B8_32x2";
	case CompressedPixelFormat::IMGIC_R16G16B16_8x8: return "IMGIC_R16G16B16_8x8";
	case CompressedPixelFormat::IMGIC_R16G16B16_16x4: return "IMGIC_R16G16B16_16x4";
	case CompressedPixelFormat::IMGIC_R16G16B16_32x2: return "IMGIC_R16G16B16_32x2";
	case CompressedPixelFormat::IMGIC_R32G32B32_8x8: return "IMGIC_R32G32B32_8x8";
	case CompressedPixelFormat::IMGIC_R32G32B32_16x4: return "IMGIC_R32G32B32_16x4";
	case CompressedPixelFormat::IMGIC_R32G32B32_32x2: return "IMGIC_R32G32B32_32x2";
	case CompressedPixelFormat::IMGIC_R8G8B8A8_8x8: return "IMGIC_R8G8B8A8_8x8";
	case CompressedPixelFormat::IMGIC_R8G8B8A8_16x4: return "IMGIC_R8G8B8A8_16x4";
	case CompressedPixelFormat::IMGIC_R8G8B8A8_32x2: return "IMGIC_R8G8B8A8_32x2";
	case CompressedPixelFormat::IMGIC_R16G16B16A16_8x8: return "IMGIC_R16G16B16A16_8x8";
	case CompressedPixelFormat::IMGIC_R16G16B16A16_16x4: return "IMGIC_R16G16B16A16_16x4";
	case CompressedPixelFormat::IMGIC_R16G16B16A16_32x2: return "IMGIC_R16G16B16A16_32x2";
	case CompressedPixelFormat::IMGIC_R32G32B32A32_8x8: return "IMGIC_R32G32B32A32_8x8";
	case CompressedPixelFormat::IMGIC_R32G32B32A32_16x4: return "IMGIC_R32G32B32A32_16x4";
	case CompressedPixelFormat::IMGIC_R32G32B32A32_32x2: return "IMGIC_R32G32B32A32_32x2";
	case CompressedPixelFormat::IMGIC_A4R4G4B4_8x8: return "IMGIC_A4R4G4B4_8x8";
	case CompressedPixelFormat::IMGIC_A4R4G4B4_16x4: return "IMGIC_A4R4G4B4_16x4";
	case CompressedPixelFormat::IMGIC_A4R4G4B4_32x2: return "IMGIC_A4R4G4B4_32x2";
	case CompressedPixelFormat::IMGIC_A8R3G3B2_8x8: return "IMGIC_A8R3G3B2_8x8";
	case CompressedPixelFormat::IMGIC_A8R3G3B2_16x4: return "IMGIC_A8R3G3B2_16x4";
	case CompressedPixelFormat::IMGIC_A8R3G3B2_32x2: return "IMGIC_A8R3G3B2_32x2";
	case CompressedPixelFormat::IMGIC_A1R5G5B5_8x8: return "IMGIC_A1R5G5B5_8x8";
	case CompressedPixelFormat::IMGIC_A1R5G5B5_16x4: return "IMGIC_A1R5G5B5_16x4";
	case CompressedPixelFormat::IMGIC_A1R5G5B5_32x2: return "IMGIC_A1R5G5B5_32x2";
	case CompressedPixelFormat::IMGIC_R5G5B5A1_8x8: return "IMGIC_R5G5B5A1_8x8";
	case CompressedPixelFormat::IMGIC_R5G5B5A1_16x4: return "IMGIC_R5G5B5A1_16x4";
	case CompressedPixelFormat::IMGIC_R5G5B5A1_32x2: return "IMGIC_R5G5B5A1_32x2";
	case CompressedPixelFormat::IMGIC_R5G6B5_8x8: return "IMGIC_R5G6B5_8x8";
	case CompressedPixelFormat::IMGIC_R5G6B5_16x4: return "IMGIC_R5G6B5_16x4";
	case CompressedPixelFormat::IMGIC_R5G6B5_32x2: return "IMGIC_R5G6B5_32x2";
	case CompressedPixelFormat::IMGIC_A2B10G10R10_8x8: return "IMGIC_A2B10G10R10_8x8";
	case CompressedPixelFormat::IMGIC_A2B10G10R10_16x4: return "IMGIC_A2B10G10R10_16x4";
	case CompressedPixelFormat::IMGIC_A2B10G10R10_32x2: return "IMGIC_A2B10G10R10_32x2";
	case CompressedPixelFormat::IMGIC_R10B10G10A2_8x8: return "IMGIC_R10B10G10A2_8x8";
	case CompressedPixelFormat::IMGIC_R10B10G10A2_16x4: return "IMGIC_R10B10G10A2_16x4";
	case CompressedPixelFormat::IMGIC_R10B10G10A2_32x2: return "IMGIC_R10B10G10A2_32x2";
	case CompressedPixelFormat::IMGIC_X8D24_8x8: return "IMGIC_X8D24_8x8";
	case CompressedPixelFormat::IMGIC_X8D24_16x4: return "IMGIC_X8D24_16x4";
	case CompressedPixelFormat::IMGIC_X8D24_32x2: return "IMGIC_X8D24_32x2";
	case CompressedPixelFormat::IMGIC_S8D24_8x8: return "IMGIC_S8D24_8x8";
	case CompressedPixelFormat::IMGIC_S8D24_16x4: return "IMGIC_S8D24_16x4";
	case CompressedPixelFormat::IMGIC_S8D24_32x2: return "IMGIC_S8D24_32x2";
	case CompressedPixelFormat::IMGIC_S8X24_8x8: return "IMGIC_S8X24_8x8";
	case CompressedPixelFormat::IMGIC_S8X24_16x4: return "IMGIC_S8X24_16x4";
	case CompressedPixelFormat::IMGIC_S8X24_32x2: return "IMGIC_S8X24_32x2";
	case CompressedPixelFormat::IMGIC_D24S8_8x8: return "IMGIC_D24S8_8x8";
	case CompressedPixelFormat::IMGIC_D24S8_16x4: return "IMGIC_D24S8_16x4";
	case CompressedPixelFormat::IMGIC_D24S8_32x2: return "IMGIC_D24S8_32x2";
	case CompressedPixelFormat::IMGIC_R9G9B9E5_8x8: return "IMGIC_R9G9B9E5_8x8";
	case CompressedPixelFormat::IMGIC_R9G9B9E5_16x4: return "IMGIC_R9G9B9E5_16x4";
	case CompressedPixelFormat::IMGIC_R9G9B9E5_32x2: return "IMGIC_R9G9B9E5_32x2";
	case CompressedPixelFormat::IMGIC_R11G11B10_8x8: return "IMGIC_R11G11B10_8x8";
	case CompressedPixelFormat::IMGIC_R11G11B10_16x4: return "IMGIC_R11G11B10_16x4";
	case CompressedPixelFormat::IMGIC_R11G11B10_32x2: return "IMGIC_R11G11B10_32x2";
	case CompressedPixelFormat::IMGIC_B10G11R11_8x8: return "IMGIC_B10G11R11_8x8";
	case CompressedPixelFormat::IMGIC_B10G11R11_16x4: return "IMGIC_B10G11R11_16x4";
	case CompressedPixelFormat::IMGIC_B10G11R11_32x2: return "IMGIC_B10G11R11_32x2";
	case CompressedPixelFormat::IMGIC_B8G8R8A8_8x8: return "IMGIC_B8G8R8A8_8x8";
	case CompressedPixelFormat::IMGIC_B8G8R8A8_16x4: return "IMGIC_B8G8R8A8_16x4";
	case CompressedPixelFormat::IMGIC_B8G8R8A8_32x2: return "IMGIC_B8G8R8A8_32x2";
	case CompressedPixelFormat::IMGIC_R8G8B8G8_8x8: return "IMGIC_R8G8B8G8_8x8";
	case CompressedPixelFormat::IMGIC_R8G8B8G8_16x4: return "IMGIC_R8G8B8G8_16x4";
	case CompressedPixelFormat::IMGIC_R8G8B8G8_32x2: return "IMGIC_R8G8B8G8_32x2";
	case CompressedPixelFormat::IMGIC_G8R8G8B8_8x8: return "IMGIC_G8R8G8B8_8x8";
	case CompressedPixelFormat::IMGIC_G8R8G8B8_16x4: return "IMGIC_G8R8G8B8_16x4";
	case CompressedPixelFormat::IMGIC_G8R8G8B8_32x2: return "IMGIC_G8R8G8B8_32x2";
	case CompressedPixelFormat::IMGIC_UYVY_8x8: return "IMGIC_UYVY_8x8";
	case CompressedPixelFormat::IMGIC_UYVY_16x4: return "IMGIC_UYVY_16x4";
	case CompressedPixelFormat::IMGIC_UYVY_32x2: return "IMGIC_UYVY_32x2";
	case CompressedPixelFormat::IMGIC_YUYV_8x8: return "IMGIC_YUYV_8x8";
	case CompressedPixelFormat::IMGIC_YUYV_16x4: return "IMGIC_YUYV_16x4";
	case CompressedPixelFormat::IMGIC_YUYV_32x2: return "IMGIC_YUYV_32x2";
	case CompressedPixelFormat::IMGIC_YUV_2P_444_8x8: return "IMGIC_YUV_2P_444_8x8";
	case CompressedPixelFormat::IMGIC_YUV_2P_444_16x4: return "IMGIC_YUV_2P_444_16x4";
	case CompressedPixelFormat::IMGIC_YUV_2P_444_32x2: return "IMGIC_YUV_2P_444_32x2";
	case CompressedPixelFormat::IMGIC_YVU_2P_444_8x8: return "IMGIC_YVU_2P_444_8x8";
	case CompressedPixelFormat::IMGIC_YVU_2P_444_16x4: return "IMGIC_YVU_2P_444_16x4";
	case CompressedPixelFormat::IMGIC_YVU_2P_444_32x2: return "IMGIC_YVU_2P_444_32x2";
	case CompressedPixelFormat::IMGIC_YVU10_2P_444_PK16_8x8: return "IMGIC_YVU10_2P_444_PK16_8x8";
	case CompressedPixelFormat::IMGIC_YVU10_2P_444_PK16_16x4: return "IMGIC_YVU10_2P_444_PK16_16x4";
	case CompressedPixelFormat::IMGIC_YVU10_2P_444_PK16_32x2: return "IMGIC_YVU10_2P_444_PK16_32x2";
	case CompressedPixelFormat::IMGIC_YUV_2P_422_8x8: return "IMGIC_YUV_2P_422_8x8";
	case CompressedPixelFormat::IMGIC_YUV_2P_422_16x4: return "IMGIC_YUV_2P_422_16x4";
	case CompressedPixelFormat::IMGIC_YUV_2P_422_32x2: return "IMGIC_YUV_2P_422_32x2";
	case CompressedPixelFormat::IMGIC_YVU_2P_422_8x8: return "IMGIC_YVU_2P_422_8x8";
	case CompressedPixelFormat::IMGIC_YVU_2P_422_16x4: return "IMGIC_YVU_2P_422_16x4";
	case CompressedPixelFormat::IMGIC_YVU_2P_422_32x2: return "IMGIC_YVU_2P_422_32x2";
	case CompressedPixelFormat::IMGIC_YVU10_2P_422_PK16_8x8: return "IMGIC_YVU10_2P_422_PK16_8x8";
	case CompressedPixelFormat::IMGIC_YVU10_2P_422_PK16_16x4: return "IMGIC_YVU10_2P_422_PK16_16x4";
	case CompressedPixelFormat::IMGIC_YVU10_2P_422_PK16_32x2: return "IMGIC_YVU10_2P_422_PK16_32x2";
	case CompressedPixelFormat::IMGIC_YUV_2P_420_8x8: return "IMGIC_YUV_2P_420_8x8";
	case CompressedPixelFormat::IMGIC_YUV_2P_420_16x4: return "IMGIC_YUV_2P_420_16x4";
	case CompressedPixelFormat::IMGIC_YUV_2P_420_32x2: return "IMGIC_YUV_2P_420_32x2";
	case CompressedPixelFormat::IMGIC_YVU_2P_420_8x8: return "IMGIC_YVU_2P_420_8x8";
	case CompressedPixelFormat::IMGIC_YVU_2P_420_16x4: return "IMGIC_YVU_2P_420_16x4";
	case CompressedPixelFormat::IMGIC_YVU_2P_420_32x2: return "IMGIC_YVU_2P_420_32x2";
	case CompressedPixelFormat::IMGIC_YVU10_2P_420_PK16_8x8: return "IMGIC_YVU10_2P_420_PK16_8x8";
	case CompressedPixelFormat::IMGIC_YVU10_2P_420_PK16_16x4: return "IMGIC_YVU10_2P_420_PK16_16x4";
	case CompressedPixelFormat::IMGIC_YVU10_2P_420_PK16_32x2: return "IMGIC_YVU10_2P_420_PK16_32x2";
	case CompressedPixelFormat::IMGIC_YUV_3P_444_8x8: return "IMGIC_YUV_3P_444_8x8";
	case CompressedPixelFormat::IMGIC_YUV_3P_444_16x4: return "IMGIC_YUV_3P_444_16x4";
	case CompressedPixelFormat::IMGIC_YUV_3P_444_32x2: return "IMGIC_YUV_3P_444_32x2";
	case CompressedPixelFormat::IMGIC_YUV10_3P_444_PK16_8x8: return "IMGIC_YUV10_3P_444_PK16_8x8";
	case CompressedPixelFormat::IMGIC_YUV10_3P_444_PK16_16x4: return "IMGIC_YUV10_3P_444_PK16_16x4";
	case CompressedPixelFormat::IMGIC_YUV10_3P_444_PK16_32x2: return "IMGIC_YUV10_3P_444_PK16_32x2";
	case CompressedPixelFormat::IMGIC_YUV_3P_420_8x8: return "IMGIC_YUV_3P_420_8x8";
	case CompressedPixelFormat::IMGIC_YUV_3P_420_16x4: return "IMGIC_YUV_3P_420_16x4";
	case CompressedPixelFormat::IMGIC_YUV_3P_420_32x2: return "IMGIC_YUV_3P_420_32x2";
	case CompressedPixelFormat::IMGIC_YVU_3P_420_8x8: return "IMGIC_YVU_3P_420_8x8";
	case CompressedPixelFormat::IMGIC_YVU_3P_420_16x4: return "IMGIC_YVU_3P_420_16x4";
	case CompressedPixelFormat::IMGIC_YVU_3P_420_32x2: return "IMGIC_YVU_3P_420_32x2";

	default: return "UNKNOWN_FORMAT";
	}
}

/// <summary>Enumeration of Colorspaces (Linear, SRGB).</summary>
enum class ColorSpace
{
	lRGB, //!< Linear RGB colorspace
	sRGB, //!< sRGB colorspace
	NumSpaces
};

inline std::string to_string(ColorSpace colorspace)
{
	switch (colorspace)
	{
	case ColorSpace::lRGB: return "LinearRGB";
	case ColorSpace::sRGB: return "sRGB";
	default: return "UNKNOWN_COLORSPACE";
	}
};

/// <summary>Enumeration of Datatypes.</summary>
enum class VariableType
{
	UnsignedByteNorm,
	SignedByteNorm,
	UnsignedByte,
	SignedByte,
	UnsignedShortNorm,
	SignedShortNorm,
	UnsignedShort,
	SignedShort,
	UnsignedIntegerNorm,
	SignedIntegerNorm,
	UnsignedInteger,
	SignedInteger,
	SignedFloat,
	Float = SignedFloat, // the name PVRFloat is now deprecated.
	UnsignedFloat,
	NumVarTypes
};

inline std::string to_string(VariableType variableType)
{
	switch (variableType)
	{
	case VariableType::UnsignedByteNorm: return "UnsignedByte_Normalized";
	case VariableType::SignedByteNorm: return "SignedByte_Normalized";
	case VariableType::UnsignedByte: return "UnsignedByte_Integer";
	case VariableType::SignedByte: return "SignedByte_Integer";
	case VariableType::UnsignedShortNorm: return "UnsignedShort_Normalized";
	case VariableType::SignedShortNorm: return "SignedShort_Normalized";
	case VariableType::UnsignedShort: return "UnsignedShort_Integer";
	case VariableType::SignedShort: return "UnsignedShort_Integer";
	case VariableType::UnsignedIntegerNorm: return "UnsignedInteger_Normalized";
	case VariableType::SignedIntegerNorm: return "SignedInteger_Normalized";
	case VariableType::UnsignedInteger: return "UnsignedInteger_Integer";
	case VariableType::SignedInteger: return "SignedInteger_Integer";
	case VariableType::SignedFloat: return "SignedFloat";
	case VariableType::UnsignedFloat: return "UnsignedFloat";
	default: return "UNKNOWN_VARIABLE_TYPE";
	}
};

/// <summary>Check if a variable type is a Signed type</summary>
/// <param name="item">The variable to check</param>
/// <returns>True if item is a signed type (signed integer, signed float etc), otherwise false</returns>
inline bool isVariableTypeSigned(VariableType item) { return static_cast<uint32_t>(item) < 11 ? static_cast<uint32_t>(item) & 1 : static_cast<uint32_t>(item) != 13; }

/// <summary>Check if a variable type is a Normalizedtype</summary>
/// <param name="item">The variable to check</param>
/// <returns>True if item is a normalized type (nomralized short, normalized integer etc)</returns>
inline bool isVariableTypeNormalized(VariableType item) { return (static_cast<uint32_t>(item) < 10) && !(static_cast<uint32_t>(item) & 2); }

/// <summary>The PixelFormat class fully defines a Pixel Format (channels, format, compression, bit width etc.).</summary>
class PixelFormat
{
public:
	/// <summary>64 bit Integer representation as 32 lower bits and 32 higher bits</summary>
	struct LowHigh
	{
		uint32_t Low; //!< Lower 32-bits of the pixel format storage
		uint32_t High; //!< Higher 32-bits of the pixel format storage
	};

	/// <summary>Default Constructor. Creates an empty pixeltype.</summary>
	PixelFormat() {}

	/// <summary>Initializes a new pixel type from a 64 bit Integer value.</summary>
	/// <param name="type">Pixel format type</param>
	/// <returns>Return a new PixelFormat</returns>
	PixelFormat(uint64_t type) : _format(type) {}

	/// <summary>Initializes a new pixel type from a CompressedPixelFormat type.</summary>
	/// <param name="type">Compressed Pixel Format type</param>
	/// <returns>Return a new PixelFormat</returns>
	PixelFormat(CompressedPixelFormat type) : _format(static_cast<uint64_t>(type)) {}

	/// <summary>Construct a Pixel format from the given channels which takes up to 4 characters (CnName) and 4 values
	/// (CnBits). Any unused channels should be set to 0.</summary>
	/// <param name="C1Name">channel 1 name</param>
	/// <param name="C2Name">channel 2 name</param>
	/// <param name="C3Name">channel 3 name</param>
	/// <param name="C4Name">channel 4 name</param>
	/// <param name="C1Bits">number of bits in channel 1</param>
	/// <param name="C2Bits">number of bits in channel 2</param>
	/// <param name="C3Bits">number of bits in channel 3</param>
	/// <param name="C4Bits">number of bits in channel 4</param>
	/// <remarks>For example: PixelFormat('r','g','b',0,8,8,8,0)</remarks>
	PixelFormat(uint8_t C1Name, uint8_t C2Name, uint8_t C3Name, uint8_t C4Name, uint8_t C1Bits, uint8_t C2Bits, uint8_t C3Bits, uint8_t C4Bits)
		: _format(C1Name, C2Name, C3Name, C4Name, C1Bits, C2Bits, C3Bits, C4Bits)
	{}

	/// <summary>Returns the "content", or "name" of a channel, as a character. (normally r,g,b,a,d,s,l,i)</summary>
	/// <param name="channel">The zero-indexed channel of the texture(0, 1, 2, 3)</param>
	/// <returns>Return a character describing the channel contents</returns>
	/// <remarks>For example, the format d24s8 would return 'd' for channel:0, 's' for channel:1, NULL otherwise</remarks>
	char getChannelContent(uint8_t channel) const
	{
		if (channel >= 4) { return 0; }
		return static_cast<char>(_format._pixelTypeChar[channel]);
	}

	/// <summary>Get the width of the specified channel</summary>
	/// <param name="channel">The zero-indexed channel of the texture(0, 1, 2, 3)</param>
	/// <returns>Return The number of bits the specified channel takes up.</returns>
	uint8_t getChannelBits(uint8_t channel) const
	{
		if (channel >= 4) { return 0; }
		return _format._pixelTypeChar[channel + 4];
	}

	/// <summary>Get the number of channels in the format.</summary>
	/// <returns>Return the number of channels in the format.</returns>
	uint8_t getNumChannels() const
	{
		return _format._pixelTypeChar[7] ? 4u : _format._pixelTypeChar[6] ? 3u : _format._pixelTypeChar[5] ? 2u : _format._pixelTypeChar[4] ? 1u : 0u;
	}

	/// <summary>Returns true if the format is a "normal" compressed format, i.e. the format is not regular (channel type/
	/// bitrate combination), but excludes some special packed formats that are not compressed, such as shared
	/// exponent formats.</summary>
	/// <returns>True if it is a compressed format, otherwise false</returns>
	bool isCompressedFormat() const { return ((_format.Part.High == 0) && (_format.Part.Low != static_cast<uint32_t>(CompressedPixelFormat::SharedExponentR9G9B9E5))) != 0; }

	/// <summary>Returns if the format is some kind of directly supported format that is not regular (i.e. channel type/
	/// channel bitrate combination). I.e. returns true if the format is any of the formats described in the supported
	/// "compressed" formats enumeration.</summary>
	/// <returns>True if it is a "simple", "regular" format (i.e. 1-4 channels of some bit width), i.e. neither a "compressed"
	/// format nor a "special" format such as shared exponents etc.</returns>
	uint8_t isIrregularFormat() const { return _format.Part.High == 0; }

	/// <summary>Get the pixel type id</summary>
	/// <returns>Return the pixel type id</returns>
	uint64_t getPixelTypeId() const { return _format._pixelTypeID; }

	/// <summary>Get a const pointer to the pixel type char</summary>
	/// <returns>Return a const pointer to the pixel type char</returns>
	const uint8_t* getPixelTypeChar() const { return _format._pixelTypeChar; }

	/// <summary>Get a pointer to the pixel type char</summary>
	/// <returns>Return a pointer to the pixel type char</returns>
	uint8_t* getPixelTypeChar() { return _format._pixelTypeChar; }

	/// <summary>Get the pixel format's low and high part</summary>
	/// <returns>Return pixel format's low and high part</returns>
	LowHigh getPart() const { return _format.Part; }

	/// <summary>Get the number of bits per pixel</summary>
	/// <returns>Return the number of bits per pixel</returns>
	uint8_t getBitsPerPixel() const
	{
		return (_format.Part.High == 0 && _format.Part.Low == static_cast<uint32_t>(CompressedPixelFormat::SharedExponentR9G9B9E5))
			? 32u
			: _format._pixelTypeChar[4u] + _format._pixelTypeChar[5u] + _format._pixelTypeChar[6u] + _format._pixelTypeChar[7u];
	}

	/// <summary>operator==, validate if a given pixel format is same as this.</summary>
	/// <param name="rhs">pixel format to compare</param>
	/// <returns>Return true if the given pixel format is same</returns>
	bool operator==(const PixelFormat& rhs) const { return getPixelTypeId() == rhs.getPixelTypeId(); }

	/// <summary>operator!=, validate if a give pixel format is not same as this.</summary>
	/// <param name="rhs">pixel format to compare</param>
	/// <returns>Return true if the given pixel format is not same</returns>
	bool operator!=(const PixelFormat& rhs) const { return !(*this == rhs); }

private:
	union PixelFormatImpl
	{
		/// <summary>Creates an empty pixeltype.</summary>
		/// <returns>A new PixelFormat</returns>
		PixelFormatImpl() : _pixelTypeID(0) {}

		/// <summary>Initializes a new pixel type from a 64 bit Integer value.</summary>
		/// <param name="type">The pixel format represented as a 64 bit Integer. The value is the same as if you get
		/// _pixelTypeID of this class.</param>
		/// <returns>A new PixelFormat</returns>
		PixelFormatImpl(uint64_t type) : _pixelTypeID(type) {}

		/************************************************************************
		\brief  Takes up to 4 characters (CnName) and 4 values (CnBits)
		to create a new PixelFormat. Any unused channels should be set to 0.
		For example: PixelFormat('r','g','b',0,8,8,8,0);
		  \param[in]      C1Name
		  \param[in]      C2Name
		  \param[in]      C3Name
		  \param[in]      C4Name
		  \param[in]      C1Bits
		  \param[in]      C2Bits
		  \param[in]      C3Bits
		  \param[in]      C4Bits
		  \return   A new PixelFormat
		*************************************************************************/
		PixelFormatImpl(uint8_t C1Name, uint8_t C2Name, uint8_t C3Name, uint8_t C4Name, uint8_t C1Bits, uint8_t C2Bits, uint8_t C3Bits, uint8_t C4Bits)
		{
			_pixelTypeChar[0] = C1Name;
			_pixelTypeChar[1] = C2Name;
			_pixelTypeChar[2] = C3Name;
			_pixelTypeChar[3] = C4Name;
			_pixelTypeChar[4] = C1Bits;
			_pixelTypeChar[5] = C2Bits;
			_pixelTypeChar[6] = C3Bits;
			_pixelTypeChar[7] = C4Bits;
		}

		LowHigh Part;
		uint64_t _pixelTypeID;
		uint8_t _pixelTypeChar[8];
	};
	PixelFormatImpl _format;

public:
	/// <summary>Retrieves a pixel format for Intensity 8.</summary>
	/// <returns>An 8 bit intensity value</returns>
	static const PixelFormat Intensity8() { return PixelFormat('i', '\0', '\0', '\0', 8, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for red 8.</summary>
	/// <returns>An 8 bit red value</returns>
	static const PixelFormat R_8() { return PixelFormat('r', '\0', '\0', '\0', 8, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for red and green channels with 8 bits per channel.</summary>
	/// <returns>An 8 bit red and green channel</returns>
	static const PixelFormat RG_88() { return PixelFormat('r', 'g', '\0', '\0', 8, 8, 0, 0); }

	/// <summary>Retrieves a pixel format for red, green and blue channels with 8 bits per channel.</summary>
	/// <returns>An 8 bit red, green and blue channel</returns>
	static const PixelFormat RGB_888() { return PixelFormat('r', 'g', 'b', '\0', 8, 8, 8, 0); }

	/// <summary>Retrieves a pixel format for red, green, blue and alpha channels with 8 bits per channel.</summary>
	/// <returns>An 8 bit red, green, blue and alpha channel</returns>
	static const PixelFormat RGBA_8888() { return PixelFormat('r', 'g', 'b', 'a', 8, 8, 8, 8); }

	/// <summary>Retrieves a pixel format for alpha, blue, green and red channels with 8 bits per channel.</summary>
	/// <returns>An 8 bit alpha, blue, green, red channel</returns>
	static const PixelFormat ABGR_8888() { return PixelFormat('a', 'b', 'g', 'r', 8, 8, 8, 8); }

	/// <summary>Retrieves a pixel format for red with 16 bits.</summary>
	/// <returns>An 16 bit red value</returns>
	static const PixelFormat R_16() { return PixelFormat('r', '\0', '\0', '\0', 16, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for red and green channels with 16 bits per channel.</summary>
	/// <returns>An 16 bit red and green channel</returns>
	static const PixelFormat RG_1616() { return PixelFormat('r', 'g', '\0', '\0', 16, 16, 0, 0); }

	/// <summary>Retrieves a pixel format for red with 32 bits.</summary>
	/// <returns>An 32 bit red value</returns>
	static const PixelFormat R_32() { return PixelFormat('r', '\0', '\0', '\0', 32, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for red and green channels with 32 bits per channel.</summary>
	/// <returns>An 32 bit red and green channel</returns>
	static const PixelFormat RG_3232() { return PixelFormat('r', 'g', '\0', '\0', 32, 32, 0, 0); }

	/// <summary>Retrieves a pixel format for red, green and blue channels with 32 bits per channel.</summary>
	/// <returns>An 32 bit red, green and blue channel</returns>
	static const PixelFormat RGB_323232() { return PixelFormat('r', 'g', 'b', '\0', 32, 32, 32, 0); }

	/// <summary>Retrieves a pixel format for red, green, blue and alpha channels with 32 bits per channel.</summary>
	/// <returns>An 32 bit red, green, blue and alpha channel</returns>
	static const PixelFormat RGBA_32323232() { return PixelFormat('r', 'g', 'b', 'a', 32, 32, 32, 32); }

	/// <summary>Retrieves a pixel format for red, green and blue channels with 5 bits for the red and blue channels and 6 bits for the green channel.</summary>
	/// <returns>An red, green and blue pixel format with 5 bits for the red and blue channels and 6 bits for the green channel</returns>
	static const PixelFormat RGB_565() { return PixelFormat('r', 'g', 'b', '\0', 5, 6, 5, 0); }

	/// <summary>Retrieves a pixel format for red, green, blue and alpha channels with 4 bits per channel.</summary>
	/// <returns>An 4 bit red, green, blue and alpha channel</returns>
	static const PixelFormat RGBA_4444() { return PixelFormat('r', 'g', 'b', 'a', 4, 4, 4, 4); }

	/// <summary>Retrieves a pixel format for red, green, blue and alpha channels with 5 bits per channel for the red, green and blue channels with 1 bit for alpha.</summary>
	/// <returns>An 32 bit red, green, blue and alpha channel</returns>
	static const PixelFormat RGBA_5551() { return PixelFormat('r', 'g', 'b', 'a', 5, 5, 5, 1); }

	/// <summary>Retrieves a pixel format for blue, green and red channels with 8 bits per channel.</summary>
	/// <returns>An 8 bit blue, green and red channel pixel format</returns>
	static const PixelFormat BGR_888() { return PixelFormat('b', 'g', 'r', '\0', 8, 8, 8, 0); }

	/// <summary>Retrieves a pixel format for blue, green, red and alpha channels with 8 bits per channel.</summary>
	/// <returns>An 8 bit blue, green, red and alpha channel pixel format</returns>
	static const PixelFormat BGRA_8888() { return PixelFormat('b', 'g', 'r', 'a', 8, 8, 8, 8); }

	/// <summary>Retrieves a pixel format for depth with 8 bits.</summary>
	/// <returns>An 8 bit depth channel pixel format</returns>
	static const PixelFormat Depth8() { return PixelFormat('d', '\0', '\0', '\0', 8, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for depth with 16 bits.</summary>
	/// <returns>An 16 bit depth channel pixel format</returns>
	static const PixelFormat Depth16() { return PixelFormat('d', '\0', '\0', '\0', 16, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for depth with 24 bits.</summary>
	/// <returns>An 24 bit depth channel pixel format</returns>
	static const PixelFormat Depth24() { return PixelFormat('d', '\0', '\0', '\0', 24, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for depth with 32 bits.</summary>
	/// <returns>An 32 bit depth channel pixel format</returns>
	static const PixelFormat Depth32() { return PixelFormat('d', '\0', '\0', '\0', 32, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for depth with 16 bits and stencil with 8 bits.</summary>
	/// <returns>A 16 bit depth channel with 8 bits for stencil pixel format</returns>
	static const PixelFormat Depth16Stencil8() { return PixelFormat('d', 's', '\0', '\0', 16, 8, 0, 0); }

	/// <summary>Retrieves a pixel format for depth with 24 bits and stencil with 8 bits.</summary>
	/// <returns>A 24 bit depth channel with 8 bits for stencil pixel format</returns>
	static const PixelFormat Depth24Stencil8() { return PixelFormat('d', 's', '\0', '\0', 24, 8, 0, 0); }

	/// <summary>Retrieves a pixel format for depth with 32 bits and stencil with 8 bits.</summary>
	/// <returns>A 32 bit depth channel with 8 bits for stencil pixel format</returns>
	static const PixelFormat Depth32Stencil8() { return PixelFormat('d', 's', '\0', '\0', 32, 8, 0, 0); }

	/// <summary>Retrieves a pixel format for stencil with 8 bits.</summary>
	/// <returns>An 8 bit stencil channel pixel format</returns>
	static const PixelFormat Stencil8() { return PixelFormat('s', '\0', '\0', '\0', 8, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for luminance with 32 bits.</summary>
	/// <returns>An 32 bit luminance channel pixel format</returns>
	static const PixelFormat L_32() { return PixelFormat('l', '\0', '\0', '\0', 32, 0, 0, 0); }

	/// <summary>Retrieves a pixel format for luminance and alpha channels with 16 bits per channel.</summary>
	/// <returns>An 16 bit luminance and alpha channel pixel format</returns>
	static const PixelFormat LA_1616() { return PixelFormat('l', 'a', '\0', '\0', 16, 16, 0, 0); }

	/// <summary>Retrieves a pixel format for luminance and alpha channels with 32 bits per channel.</summary>
	/// <returns>An 32 bit luminance and alpha channel pixel format</returns>
	static const PixelFormat LA_3232() { return PixelFormat('l', 'a', '\0', '\0', 32, 32, 0, 0); }

	/// <summary>Retrieves a pixel format for red, green, blue and alpha channels with 16 bits per channel.</summary>
	/// <returns>An 16 bit red, green, blue and alpha channel</returns>
	static const PixelFormat RGBA_16161616() { return PixelFormat('r', 'g', 'b', 'a', 16, 16, 16, 16); }

	/// <summary>Retrieves a pixel format for red, green and blue channels with 16 bits per channel.</summary>
	/// <returns>An 16 bit red, green and blue channel</returns>
	static const PixelFormat RGB_161616() { return PixelFormat('r', 'g', 'b', '\0', 16, 16, 16, 0); }

	/// <summary>Retrieves a packed pixel format for blue, green and red channels with 10 bits for the blue and 11 bits for the green and red channel. This pixel format is
	/// identical to BGR111110.</summary> <returns>An blue, green and red pixel format with 10 bits for the blue and 11 bits for the green and red channel</returns>
	static const PixelFormat RGB_111110() { return PixelFormat('b', 'g', 'r', '\0', 10, 11, 11, 0); }

	/// <summary>Retrieves a packed pixel format for blue, green and red channels with 10 bits for the blue and 11 bits for the green and red channel. This pixel format is
	/// identical to RGB111110.</summary> <returns>An blue, green and red pixel format with 10 bits for the blue and 11 bits for the green and red channel</returns>
	static const PixelFormat BGR_101111() { return RGB_111110(); }

	/// <summary>Retrieves an unknown pixel format.</summary>
	/// <returns>An unknown pixel format</returns>
	static const PixelFormat Unknown() { return PixelFormat(0, 0, 0, 0, 0, 0, 0, 0); }
};

inline std::string to_string(const PixelFormat& fmt)
{
	if (fmt.getPart().High == 0) { return to_string(static_cast<CompressedPixelFormat>(fmt.getPixelTypeId())); }
	std::stringstream ss;
	for (uint8_t i = 0; i < 4; ++i)
	{
		if (fmt.getChannelContent(i) != 0)
		{
			ss << fmt.getChannelContent(i);
			ss << fmt.getChannelBits(i);
		}
	}
	return ss.str();
}

/// <summary>Use this template class to generate a 4 channel PixelID.</summary>
/// <param name="C1Name">The Name of the 1st channel (poss. values 'r','g','b','a','l',0)</param>
/// <param name="C2Name">The Name of the 2nd channel (poss. values 'r','g','b','a','l',0)</param>
/// <param name="C3Name">The Name of the 3rd channel (poss. values 'r','g','b','a','l',0)</param>
/// <param name="C4Name">The Name of the 4th channel (poss. values 'r','g','b','a','l',0)</param>
/// <param name="C1Bits">The number of bits of the 1st channel</param>
/// <param name="C2Bits">The number of bits of the 2nd channel</param>
/// <param name="C3Bits">The number of bits of the 3rd channel</param>
/// <param name="C4Bits">The number of bits of the 4th channel</param>
/// <remarks>Use this template class to generate a 4 channel PixelID (64-bit identifier for a pixel format used
/// throughout PVR Assets from the channel information. Simply define the template parameters for your class and
/// get the ID member. EXAMPLE USE: <code>uint64_t myPixelID = GeneratePixelType4&lt;'b','g','r','a',8,8,8,8&gt::ID;
/// </code></remarks>
template<char C1Name, char C2Name, char C3Name, char C4Name, uint8_t C1Bits, uint8_t C2Bits, uint8_t C3Bits, uint8_t C4Bits>
class GeneratePixelType4
{
public:
	/// <summary>A 64 bit integer uniquely representing this pixel type</summary>
	static const uint64_t ID = (static_cast<uint64_t>(C1Name) + (static_cast<uint64_t>(C2Name) << 8) + (static_cast<uint64_t>(C3Name) << 16) + (static_cast<uint64_t>(C4Name) << 24) +
		(static_cast<uint64_t>(C1Bits) << 32) + (static_cast<uint64_t>(C2Bits) << 40) + (static_cast<uint64_t>(C3Bits) << 48) + (static_cast<uint64_t>(C4Bits) << 56));
};

/// <summary>Use this template class to generate a 3 channel PixelID.</summary>
/// <param name="C1Name">The Name of the 1st channel (poss. values 'r','g','b','a','l',0)</param>
/// <param name="C2Name">The Name of the 2nd channel (poss. values 'r','g','b','a','l',0)</param>
/// <param name="C3Name">The Name of the 3rd channel (poss. values 'r','g','b','a','l',0)</param>
/// <param name="C1Bits">The number of bits of the 1st channel</param>
/// <param name="C2Bits">The number of bits of the 2nd channel</param>
/// <param name="C3Bits">The number of bits of the 3rd channel</param>
/// <remarks>Use this template class to generate a 3 channel PixelID (64-bit identifier for a pixel format used
/// throughout PVR Assets from the channel information. Simply define the template parameters for your class and
/// get the ID member. EXAMPLE USE: <code>uint64_t myPixelID = GeneratePixelType3&lt;'r','g','b',8,8,8&gt::ID</code></remarks>
template<char C1Name, char C2Name, char C3Name, uint8_t C1Bits, uint8_t C2Bits, uint8_t C3Bits>
class GeneratePixelType3
{
public:
	/// <summary>A 64 bit integer uniquely representing this pixel type</summary>
	static const uint64_t ID = (static_cast<uint64_t>(C1Name) + (static_cast<uint64_t>(C2Name) << 8) + (static_cast<uint64_t>(C3Name) << 16) + (static_cast<uint64_t>(0) << 24) +
		(static_cast<uint64_t>(C1Bits) << 32) + (static_cast<uint64_t>(C2Bits) << 40) + (static_cast<uint64_t>(C3Bits) << 48) + (static_cast<uint64_t>(0) << 56));
};

/// <summary>Use this template class to generate a 2 channel PixelID.</summary>
/// <param name="C1Name">The Name of the 1st channel (poss. values 'r','g','a','l',0)</param>
/// <param name="C2Name">The Name of the 2nd channel (poss. values 'r','g','a','l',0)</param>
/// <param name="C1Bits">The number of bits of the 1st channel</param>
/// <param name="C2Bits">The number of bits of the 2nd channel</param>
/// <remarks>Use this template class to generate a 2 channel PixelID (64-bit identifier for a pixel format used
/// throughout PVR Assets from the channel information. Simply define the template parameters for your class and
/// get the ID member. EXAMPLE USE: <code>uint64_t myPixelID = GeneratePixelType2&lt;'r','a',8,8&gt::ID</code></remarks>
template<char C1Name, char C2Name, uint8_t C1Bits, uint8_t C2Bits>
class GeneratePixelType2
{
public:
	/// <summary>A 64 bit integer uniquely representing this pixel type</summary>
	static const uint64_t ID = (static_cast<uint64_t>(C1Name) + (static_cast<uint64_t>(C2Name) << 8) + (static_cast<uint64_t>(0) << 16) + (static_cast<uint64_t>(0) << 24) +
		(static_cast<uint64_t>(C1Bits) << 32) + (static_cast<uint64_t>(C2Bits) << 40) + (static_cast<uint64_t>(0) << 48) + (static_cast<uint64_t>(0) << 56));
};

/// <summary>Use this template class to generate a 1 channel PixelID.</summary>
/// <param name="C1Name">The Name of the 1st channel (poss. values 'r','a','l',0)</param>
/// <param name="C1Bits">The number of bits of the 1st channel</param>
/// <remarks>Use this template class to generate a 1 channel PixelID (64-bit identifier for a pixel format used
/// throughout PVR Assets from the channel information. Simply define the template parameters for your class and
/// get the ID member. EXAMPLE USE: <code>uint64_t myPixelID = GeneratePixelType1&lt;'r',8&gt::ID</code></remarks>
template<char C1Name, uint8_t C1Bits>
class GeneratePixelType1
{
public:
	/// <summary>A 64 bit integer uniquely representing this pixel type</summary>
	static const uint64_t ID = (static_cast<uint64_t>(C1Name) + (static_cast<uint64_t>(0) << 8) + (static_cast<uint64_t>(0) << 16) + (static_cast<uint64_t>(0) << 24) +
		(static_cast<uint64_t>(C1Bits) << 32) + (static_cast<uint64_t>(0) << 40) + (static_cast<uint64_t>(0) << 48) + (static_cast<uint64_t>(0) << 56));
};
} // namespace pvr
