namespace vtfpp.test
{
	[TestClass]
	public class ImageFormatDetailsTest
	{
		[TestMethod]
		public void Red()
		{
			foreach (var dataTuple in RedData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.Red(format));
				}
			}
		}


		[TestMethod]
		public void DecompressedRed()
		{
			foreach (var dataTuple in DecompressedRedData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.DecompressedRed(format));
				}
			}
		}

		[TestMethod]
		public void Green()
		{
			foreach (var dataTuple in GreenData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.Green(format));
				}
			}
		}


		[TestMethod]
		public void DecompressedGreen()
		{
			foreach (var dataTuple in DecompressedGreenData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.DecompressedGreen(format));
				}
			}
		}

		[TestMethod]
		public void Blue()
		{
			foreach (var dataTuple in BlueData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.Blue(format));
				}
			}
		}


		[TestMethod]
		public void DecompressedBlue()
		{
			foreach (var dataTuple in DecompressedBlueData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.DecompressedBlue(format));
				}
			}
		}

		[TestMethod]
		public void Alpha()
		{
			foreach (var dataTuple in AlphaData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.Alpha(format));
				}
			}
		}


		[TestMethod]
		public void DecompressedAlpha()
		{
			foreach (var dataTuple in DecompressedAlphaData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.DecompressedAlpha(format));
				}
			}
		}

		[TestMethod]
		public void BPP()
		{
			foreach (var dataTuple in BPPData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual(dataTuple.ExpectedValue, ImageFormatDetails.BPP(format));
				}
			}
		}

		[TestMethod]
		public void ContainerFormat()
		{
			foreach (var dataTuple in ContainerFormatData)
			{
				foreach (var format in dataTuple.Formats)
				{
					Assert.AreEqual<ImageFormat>(dataTuple.ExpectedValue, ImageFormatDetails.ContainerFormat(format));
				}
			}
		}

		// EXPECTED DATA 
		private readonly List<(List<ImageFormat> Formats, sbyte ExpectedValue)> RedData = new()
		{
			(new List<ImageFormat> { ImageFormat.R32F, ImageFormat.RG3232F, ImageFormat.RGB323232F, ImageFormat.RGBA32323232F }, 32),
			(new List<ImageFormat> { ImageFormat.R16F, ImageFormat.RG1616F, ImageFormat.RGBA16161616F, ImageFormat.RGBA16161616, ImageFormat.CONSOLE_RGBA16161616_LINEAR }, 16),
			(new List<ImageFormat> { ImageFormat.RGBA1010102, ImageFormat.BGRA1010102 }, 10),
			(new List<ImageFormat> { ImageFormat.RGBA8888, ImageFormat.CONSOLE_RGBA8888_LINEAR, ImageFormat.ABGR8888, ImageFormat.CONSOLE_ABGR8888_LINEAR, ImageFormat.RGB888, ImageFormat.CONSOLE_RGB888_LINEAR, ImageFormat.BGR888, ImageFormat.CONSOLE_BGR888_LINEAR, ImageFormat.I8, ImageFormat.CONSOLE_I8_LINEAR, ImageFormat.IA88, ImageFormat.P8, ImageFormat.RGB888_BLUESCREEN, ImageFormat.BGR888_BLUESCREEN, ImageFormat.ARGB8888, ImageFormat.CONSOLE_ARGB8888_LINEAR, ImageFormat.BGRA8888, ImageFormat.CONSOLE_BGRA8888_LINEAR, ImageFormat.CONSOLE_BGRA8888_LE, ImageFormat.BGRX8888, ImageFormat.CONSOLE_BGRX8888_LINEAR, ImageFormat.CONSOLE_BGRX8888_LE, ImageFormat.UV88, ImageFormat.UVWQ8888, ImageFormat.UVLX8888, ImageFormat.RGBX8888, ImageFormat.R8 }, 8),
			(new List<ImageFormat> { ImageFormat.RGB565, ImageFormat.BGR565, ImageFormat.BGRX5551, ImageFormat.CONSOLE_BGRX5551_LINEAR, ImageFormat.BGRA5551 }, 5),
			(new List<ImageFormat> { ImageFormat.BGRA4444 }, 4),
			(new List<ImageFormat> { ImageFormat.A8, ImageFormat.EMPTY }, 0),
			(new List<ImageFormat> { ImageFormat.DXT1, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.DXT1_ONE_BIT_ALPHA, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.BC7, ImageFormat.BC6H }, -1)
		};

		private readonly List<(List<ImageFormat> Formats, sbyte ExpectedValue)> DecompressedRedData = new()
		{
			(new List<ImageFormat> { ImageFormat.DXT1, ImageFormat.DXT1_ONE_BIT_ALPHA, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.BC7 }, 8 ),
			(new List<ImageFormat> { ImageFormat.BC6H }, 16),
		};


		private readonly List<(List<ImageFormat> Formats, sbyte ExpectedValue)> GreenData = new()
		{
			(new List<ImageFormat> { ImageFormat.RG3232F, ImageFormat.RGB323232F, ImageFormat.RGBA32323232F, }, 32),
			(new List<ImageFormat> { ImageFormat.RG1616F, ImageFormat.RGBA16161616F, ImageFormat.RGBA16161616, ImageFormat.CONSOLE_RGBA16161616_LINEAR, }, 16),
			(new List<ImageFormat> { ImageFormat.RGBA1010102, ImageFormat.BGRA1010102, }, 10),
			(new List<ImageFormat> { ImageFormat.RGBA8888, ImageFormat.CONSOLE_RGBA8888_LINEAR, ImageFormat.ABGR8888, ImageFormat.CONSOLE_ABGR8888_LINEAR, ImageFormat.RGB888, ImageFormat.CONSOLE_RGB888_LINEAR, ImageFormat.BGR888, ImageFormat.CONSOLE_BGR888_LINEAR, ImageFormat.RGB888_BLUESCREEN, ImageFormat.BGR888_BLUESCREEN, ImageFormat.ARGB8888, ImageFormat.CONSOLE_ARGB8888_LINEAR, ImageFormat.BGRA8888, ImageFormat.CONSOLE_BGRA8888_LINEAR, ImageFormat.CONSOLE_BGRA8888_LE, ImageFormat.BGRX8888, ImageFormat.CONSOLE_BGRX8888_LINEAR, ImageFormat.CONSOLE_BGRX8888_LE, ImageFormat.UV88, ImageFormat.UVWQ8888, ImageFormat.UVLX8888, ImageFormat.RGBX8888, }, 8),
			(new List<ImageFormat> { ImageFormat.RGB565, ImageFormat.BGR565, }, 6),
			(new List<ImageFormat> { ImageFormat.BGRX5551, ImageFormat.CONSOLE_BGRX5551_LINEAR, ImageFormat.BGRA5551, }, 5),
			(new List<ImageFormat> { ImageFormat.BGRA4444, }, 4),
			(new List<ImageFormat> { ImageFormat.I8, ImageFormat.CONSOLE_I8_LINEAR, ImageFormat.IA88, ImageFormat.P8, ImageFormat.R32F, ImageFormat.A8, ImageFormat.EMPTY, ImageFormat.R16F, ImageFormat.R8, }, 0),
			(new List<ImageFormat> { ImageFormat.DXT1, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.DXT1_ONE_BIT_ALPHA, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.BC7, ImageFormat.BC6H, }, -1),
		};


		private readonly List<(List<ImageFormat> Formats, sbyte ExpectedValue)> DecompressedGreenData = new()
		{
			(new List<ImageFormat> { ImageFormat.DXT1, ImageFormat.DXT1_ONE_BIT_ALPHA, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.BC7, }, 8),
			(new List<ImageFormat> { ImageFormat.BC6H, }, 16),
		};

		private readonly List<(List<ImageFormat> Formats, sbyte ExpectedValue)> BlueData = new()
		{
			(new List<ImageFormat> { ImageFormat.RGB323232F, ImageFormat.RGBA32323232F, }, 32),
			(new List<ImageFormat> { ImageFormat.RGBA16161616F, ImageFormat.RGBA16161616, ImageFormat.CONSOLE_RGBA16161616_LINEAR, }, 16),
			(new List<ImageFormat> { ImageFormat.RGBA1010102, ImageFormat.BGRA1010102, }, 10),
			(new List<ImageFormat> { ImageFormat.RGBA8888, ImageFormat.CONSOLE_RGBA8888_LINEAR, ImageFormat.ABGR8888, ImageFormat.CONSOLE_ABGR8888_LINEAR, ImageFormat.RGB888, ImageFormat.CONSOLE_RGB888_LINEAR, ImageFormat.BGR888, ImageFormat.CONSOLE_BGR888_LINEAR, ImageFormat.RGB888_BLUESCREEN, ImageFormat.BGR888_BLUESCREEN, ImageFormat.ARGB8888, ImageFormat.CONSOLE_ARGB8888_LINEAR, ImageFormat.BGRA8888, ImageFormat.CONSOLE_BGRA8888_LINEAR, ImageFormat.CONSOLE_BGRA8888_LE, ImageFormat.BGRX8888, ImageFormat.CONSOLE_BGRX8888_LINEAR, ImageFormat.CONSOLE_BGRX8888_LE, ImageFormat.UVWQ8888, ImageFormat.UVLX8888, ImageFormat.RGBX8888, }, 8),
			(new List<ImageFormat> { ImageFormat.RGB565, ImageFormat.BGR565, ImageFormat.BGRX5551, ImageFormat.CONSOLE_BGRX5551_LINEAR, ImageFormat.BGRA5551, }, 5),
			(new List<ImageFormat> { ImageFormat.BGRA4444, }, 4),
			(new List<ImageFormat> { ImageFormat.I8, ImageFormat.CONSOLE_I8_LINEAR, ImageFormat.IA88, ImageFormat.P8, ImageFormat.UV88, ImageFormat.R32F, ImageFormat.A8, ImageFormat.EMPTY, ImageFormat.RG3232F, ImageFormat.RG1616F, ImageFormat.R16F, ImageFormat.R8, }, 0),
			(new List<ImageFormat> { ImageFormat.DXT1, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.DXT1_ONE_BIT_ALPHA, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.BC7, ImageFormat.BC6H, }, -1),
		};

		private readonly List<(List<ImageFormat> Formats, sbyte ExpectedValue)> DecompressedBlueData = new()
		{
			(new List<ImageFormat> { ImageFormat.DXT1, ImageFormat.DXT1_ONE_BIT_ALPHA, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.BC7, }, 8),
			(new List<ImageFormat> { ImageFormat.BC6H, }, 16),
		};

		private readonly List<(List<ImageFormat> Formats, sbyte ExpectedValue)> AlphaData = new()
		{
			(new List<ImageFormat> { ImageFormat.RGBA32323232F, }, 32),
			(new List<ImageFormat> { ImageFormat.RGBA16161616F, ImageFormat.RGBA16161616, ImageFormat.CONSOLE_RGBA16161616_LINEAR, }, 16),
			(new List<ImageFormat> { ImageFormat.RGBA8888, ImageFormat.CONSOLE_RGBA8888_LINEAR, ImageFormat.ABGR8888, ImageFormat.CONSOLE_ABGR8888_LINEAR, ImageFormat.IA88, ImageFormat.ARGB8888, ImageFormat.CONSOLE_ARGB8888_LINEAR, ImageFormat.BGRA8888, ImageFormat.CONSOLE_BGRA8888_LINEAR, ImageFormat.CONSOLE_BGRA8888_LE, ImageFormat.BGRX8888, ImageFormat.CONSOLE_BGRX8888_LINEAR, ImageFormat.CONSOLE_BGRX8888_LE, ImageFormat.UVWQ8888, ImageFormat.UVLX8888, ImageFormat.RGBX8888, }, 8),
			(new List<ImageFormat> { ImageFormat.BGRA4444, }, 4),
			(new List<ImageFormat> { ImageFormat.RGBA1010102, ImageFormat.BGRA1010102, }, 2),
			(new List<ImageFormat> { ImageFormat.BGRX5551, ImageFormat.CONSOLE_BGRX5551_LINEAR, ImageFormat.BGRA5551, }, 1),
			(new List<ImageFormat> { ImageFormat.RGB888, ImageFormat.CONSOLE_RGB888_LINEAR, ImageFormat.BGR888, ImageFormat.CONSOLE_BGR888_LINEAR, ImageFormat.P8, ImageFormat.I8, ImageFormat.CONSOLE_I8_LINEAR, ImageFormat.RGB888_BLUESCREEN, ImageFormat.BGR888_BLUESCREEN, ImageFormat.UV88, ImageFormat.RGB565, ImageFormat.BGR565, ImageFormat.R32F, ImageFormat.RGB323232F, ImageFormat.A8, ImageFormat.EMPTY, ImageFormat.RG3232F, ImageFormat.RG1616F, ImageFormat.R16F, ImageFormat.R8, }, 0),
			(new List<ImageFormat> { ImageFormat.DXT1, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.DXT1_ONE_BIT_ALPHA, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.BC7, ImageFormat.BC6H, }, -1),
		};

		private readonly List<(List<ImageFormat> Formats, sbyte ExpectedValue)> DecompressedAlphaData = new()
		{
			(new List<ImageFormat> { ImageFormat.DXT5, ImageFormat.BC7, }, 8),
			(new List<ImageFormat> { ImageFormat.DXT3, }, 4),
			(new List<ImageFormat> { ImageFormat.DXT1_ONE_BIT_ALPHA, }, 1),
			(new List<ImageFormat> { ImageFormat.DXT1, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.BC6H, }, 0),
		};

		private readonly List<(List<ImageFormat> Formats, byte ExpectedValue)> BPPData = new()
		{
			(new List<ImageFormat> { ImageFormat.RGBA32323232F, }, 128),
			(new List<ImageFormat> { ImageFormat.RGB323232F, }, 96),
			(new List<ImageFormat> { ImageFormat.RGBA16161616F, ImageFormat.RGBA16161616, ImageFormat.CONSOLE_RGBA16161616_LINEAR, ImageFormat.RG3232F, }, 64),
			(new List<ImageFormat> { ImageFormat.RGBA8888, ImageFormat.CONSOLE_RGBA8888_LINEAR, ImageFormat.ABGR8888, ImageFormat.CONSOLE_ABGR8888_LINEAR, ImageFormat.ARGB8888, ImageFormat.CONSOLE_ARGB8888_LINEAR, ImageFormat.BGRA8888, ImageFormat.CONSOLE_BGRA8888_LINEAR, ImageFormat.CONSOLE_BGRA8888_LE, ImageFormat.BGRX8888, ImageFormat.CONSOLE_BGRX8888_LINEAR, ImageFormat.CONSOLE_BGRX8888_LE, ImageFormat.UVLX8888, ImageFormat.R32F, ImageFormat.UVWQ8888, ImageFormat.RGBX8888, ImageFormat.RGBA1010102, ImageFormat.BGRA1010102, ImageFormat.RG1616F, }, 32),
			(new List<ImageFormat> { ImageFormat.RGB888, ImageFormat.CONSOLE_RGB888_LINEAR, ImageFormat.BGR888, ImageFormat.CONSOLE_BGR888_LINEAR, ImageFormat.RGB888_BLUESCREEN, ImageFormat.BGR888_BLUESCREEN, }, 24),
			(new List<ImageFormat> { ImageFormat.RGB565, ImageFormat.BGR565, ImageFormat.IA88, ImageFormat.BGRX5551, ImageFormat.CONSOLE_BGRX5551_LINEAR, ImageFormat.BGRA4444, ImageFormat.BGRA5551, ImageFormat.UV88, ImageFormat.R16F, }, 16),
			(new List<ImageFormat> { ImageFormat.I8, ImageFormat.CONSOLE_I8_LINEAR, ImageFormat.P8, ImageFormat.A8, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.BC7, ImageFormat.BC6H, ImageFormat.ATI2N, ImageFormat.R8, }, 8),
			(new List<ImageFormat> { ImageFormat.ATI1N, ImageFormat.DXT1, ImageFormat.DXT1_ONE_BIT_ALPHA, }, 4),
			(new List<ImageFormat> { ImageFormat.EMPTY, }, 0),
		};

		private readonly List<(List<ImageFormat> Formats, ImageFormat ExpectedValue)> ContainerFormatData = new()
		{
			(new List<ImageFormat> { ImageFormat.R32F, ImageFormat.RG3232F, ImageFormat.RGB323232F, ImageFormat.R16F, ImageFormat.RG1616F, ImageFormat.RGBA16161616F, ImageFormat.RGBA32323232F, ImageFormat.BC6H, }, ImageFormat.RGBA32323232F),
			(new List<ImageFormat> { ImageFormat.RGBA16161616, ImageFormat.CONSOLE_RGBA16161616_LINEAR, ImageFormat.RGBA1010102, ImageFormat.BGRA1010102, }, ImageFormat.RGBA16161616),
			(new List<ImageFormat> { ImageFormat.RGBA8888, ImageFormat.CONSOLE_RGBA8888_LINEAR, ImageFormat.ABGR8888, ImageFormat.CONSOLE_ABGR8888_LINEAR, ImageFormat.RGB888, ImageFormat.CONSOLE_RGB888_LINEAR, ImageFormat.BGR888, ImageFormat.CONSOLE_BGR888_LINEAR, ImageFormat.RGB888_BLUESCREEN, ImageFormat.BGR888_BLUESCREEN, ImageFormat.ARGB8888, ImageFormat.CONSOLE_ARGB8888_LINEAR, ImageFormat.BGRA8888, ImageFormat.CONSOLE_BGRA8888_LINEAR, ImageFormat.CONSOLE_BGRA8888_LE, ImageFormat.BGRX8888, ImageFormat.CONSOLE_BGRX8888_LINEAR, ImageFormat.CONSOLE_BGRX8888_LE, ImageFormat.UVWQ8888, ImageFormat.UVLX8888, ImageFormat.RGB565, ImageFormat.BGR565, ImageFormat.BGRX5551, ImageFormat.CONSOLE_BGRX5551_LINEAR, ImageFormat.BGRA5551, ImageFormat.BGRA4444, ImageFormat.I8, ImageFormat.CONSOLE_I8_LINEAR, ImageFormat.IA88, ImageFormat.P8, ImageFormat.UV88, ImageFormat.A8, ImageFormat.DXT1, ImageFormat.DXT3, ImageFormat.DXT5, ImageFormat.DXT1_ONE_BIT_ALPHA, ImageFormat.ATI2N, ImageFormat.ATI1N, ImageFormat.RGBX8888, ImageFormat.R8, ImageFormat.BC7, }, ImageFormat.RGBA8888),
			(new List<ImageFormat> { ImageFormat.EMPTY, }, ImageFormat.EMPTY),
		};
	}
}
