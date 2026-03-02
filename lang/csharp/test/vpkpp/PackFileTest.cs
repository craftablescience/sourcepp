using System.Runtime.InteropServices;
using sourcepp.vpkpp;
using sourcepp.vpkpp.Format;
using Attribute = sourcepp.vpkpp.Attribute;

namespace sourcepp.test.vpkpp;

[TestClass]
public class PackFileTest
{
	[TestMethod]
	public void Open()
	{
		var vpk = PackFile.Open(BasePortalPath + "portal_pak_dir.vpk", (path, entry) =>
		{
			Console.WriteLine(path + ' ' + entry.ArchiveIndex);
		});
		Assert.IsNotNull(vpk);

		Assert.IsFalse(vpk.IsCaseSensitive);

		var entry = vpk.FindEntry("materials/console/background1.vmt");
		Assert.IsNotNull(entry);
		Assert.AreEqual(vpk.ReadEntryText("materials/console/background1.vmt"), "\"UnlitGeneric\"\r\n{\r\n\t\"$basetexture\" \"console/background1\"\r\n\t\"$vertexcolor\" 1\r\n\t\"$vertexalpha\" 1\r\n\t\"$ignorez\" 1\r\n\t\"$no_fullbright\" \"1\"\r\n\t\"$nolod\" \"1\"\r\n}");

		Assert.IsFalse(vpk.IsReadOnly);

		Assert.AreEqual(vpk.EntryCount(), 3509u);

		Assert.AreEqual(vpk.Filepath, BasePortalPath + "portal_pak_dir.vpk");
		Assert.AreEqual(vpk.TruncatedFilepath, BasePortalPath + "portal_pak");
		Assert.AreEqual(vpk.Filename, "portal_pak_dir.vpk");
		Assert.AreEqual(vpk.TruncatedFilename, "portal_pak.vpk");
		Assert.AreEqual(vpk.Filestem, "portal_pak_dir");
		Assert.AreEqual(vpk.TruncatedFilestem, "portal_pak");
		Assert.AreEqual(vpk.SupportedEntryAttributes, Attribute.LENGTH | Attribute.VPK_PRELOADED_DATA | Attribute.ARCHIVE_INDEX | Attribute.CRC32);
		Assert.AreEqual(vpk.ToString(), "portal_pak.vpk | Version v2");
	}

	[TestMethod]
	public void VerifyChecksums()
	{
		var vpk = PackFile.Open(BasePortalPath + "portal_pak_dir.vpk");
		Assert.IsNotNull(vpk);
		Assert.AreEqual(vpk.VerifyEntryChecksums().Count(), 0);
		Assert.IsTrue(vpk.VerifyPackFileChecksum());
	}

	[TestMethod]
	public void GetVpkVersion()
	{
		var vpk = VPK.Open(BasePortalPath + "portal_pak_dir.vpk");
		Assert.IsNotNull(vpk);
		Assert.AreEqual(vpk.Version, 2u);
	}

	private static string BasePortalPath
	{
		get
		{
			if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
			{
				return "D:/SteamLibrary/steamapps/common/Portal/portal/";
			}
			if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
			{
				return Environment.GetEnvironmentVariable("HOME") + "/.steam/steam/steamapps/common/Portal/portal/";
			}
			throw new FileLoadException("Unable to find Steam install directory!");
		}
	}
}
