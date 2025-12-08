using System.Runtime.InteropServices;

namespace vtfpp.test
{
	[TestClass]
	public class VTFTest
	{
		[TestMethod]
		public void OpenFromFile()
		{
			string vtfPath = Path.Combine(BasePortalPath, "materials/signage/overlay_aperture_logo_worn.vtf");

			var vtf = VTF.OpenFromFile(vtfPath);
			Assert.IsNotNull(vtf);

			var data = vtf.GetImageDataAsRGBA8888(0, 0, 0, 0);
			Assert.IsNotNull(data);
		}

		[TestMethod]
		public void OpenFromMemory()
		{
			string vtfPath = Path.Combine(BasePortalPath, "materials/signage/overlay_aperture_logo_worn.vtf");
			byte[] vtfRawData = File.ReadAllBytes(vtfPath);


			var vtf = VTF.OpenFromMemory(vtfRawData, 0);
			Assert.IsNotNull(vtf);

			var data = vtf.GetImageDataAsRGBA8888(0, 0, 0, 0);
			Assert.IsNotNull(data);
		}

		private static string BasePortalPath
		{
			get
			{
				if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
				{
					// CHANGE BEFORE COMMIT!!!
					return @"F:/SteamLibrary/steamapps/common/Portal/portal/";
				}
				if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
				{
					return Environment.GetEnvironmentVariable("HOME") + "/.steam/steam/steamapps/common/Portal/portal/";
				}
				throw new FileLoadException("Unable to find Steam install directory!");
			}
		}
	}
}
