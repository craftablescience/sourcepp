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
		Assert.AreEqual("\"UnlitGeneric\"\r\n{\r\n\t\"$basetexture\" \"console/background1\"\r\n\t\"$vertexcolor\" 1\r\n\t\"$vertexalpha\" 1\r\n\t\"$ignorez\" 1\r\n\t\"$no_fullbright\" \"1\"\r\n\t\"$nolod\" \"1\"\r\n}", vpk.ReadEntryText("materials/console/background1.vmt"));

		Assert.IsFalse(vpk.IsReadOnly);

		Assert.AreEqual(3509u, vpk.EntryCount());

		Assert.AreEqual(BasePortalPath + "portal_pak_dir.vpk", vpk.Filepath);
		Assert.AreEqual(BasePortalPath + "portal_pak", vpk.TruncatedFilepath);
		Assert.AreEqual("portal_pak_dir.vpk", vpk.Filename);
		Assert.AreEqual("portal_pak.vpk", vpk.TruncatedFilename);
		Assert.AreEqual("portal_pak_dir", vpk.Filestem);
		Assert.AreEqual("portal_pak", vpk.TruncatedFilestem);
		Assert.AreEqual(Attribute.LENGTH | Attribute.VPK_PRELOADED_DATA | Attribute.ARCHIVE_INDEX | Attribute.CRC32, vpk.SupportedEntryAttributes);
		Assert.AreEqual("portal_pak.vpk | Version v2", vpk.ToString());
	}

	[TestMethod]
	public void VerifyChecksums()
	{
		var vpk = PackFile.Open(BasePortalPath + "portal_pak_dir.vpk");
		Assert.IsNotNull(vpk);
		Assert.IsEmpty(vpk.VerifyEntryChecksums());
		Assert.IsTrue(vpk.VerifyPackFileChecksum());
	}

	[TestMethod]
	public void GetVpkVersion()
	{
		var vpk = VPK.Open(BasePortalPath + "portal_pak_dir.vpk");
		Assert.IsNotNull(vpk);
		Assert.AreEqual(2u, vpk.Version);
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
