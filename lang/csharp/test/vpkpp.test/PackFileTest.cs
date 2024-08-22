using System.Runtime.InteropServices;

namespace vpkpp.test
{
    [TestClass]
    public class PackFileTest
    {
        [TestMethod]
        public void Open()
        {
            var vpk = PackFile.Open(BasePortalPath + "portal_pak_dir.vpk", (path, entry) =>
            {
                Console.WriteLine(path);
            });
            Assert.IsNotNull(vpk);
            Assert.AreEqual(vpk.Type, PackFileType.VPK);

            Assert.IsFalse(vpk.CaseSensitive);

            var entry = vpk.FindEntry("materials/console/background1.vmt");
            Assert.IsNotNull(entry);
            Assert.AreEqual(vpk.ReadEntryText("materials/console/background1.vmt"), "\"UnlitGeneric\"\r\n{\r\n\t\"$basetexture\" \"console/background1\"\r\n\t\"$vertexcolor\" 1\r\n\t\"$vertexalpha\" 1\r\n\t\"$ignorez\" 1\r\n\t\"$no_fullbright\" \"1\"\r\n\t\"$nolod\" \"1\"\r\n}");

            Assert.IsFalse(vpk.ReadOnly);

            Assert.AreEqual(vpk.GetEntryCount(), 3509u);

            Assert.AreEqual(vpk.FilePath, BasePortalPath + "portal_pak_dir.vpk");
            Assert.AreEqual(vpk.TruncatedFilePath, BasePortalPath + "portal_pak");
            Assert.AreEqual(vpk.FileName, "portal_pak_dir.vpk");
            Assert.AreEqual(vpk.TruncatedFileName, "portal_pak.vpk");
            Assert.AreEqual(vpk.FileStem, "portal_pak_dir");
            Assert.AreEqual(vpk.TruncatedFileStem, "portal_pak");
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
        public void GetSupportedFileTypes()
        {
            Assert.AreEqual(PackFile.GetSupportedFileTypes().Count, 12);
        }

        [TestMethod]
        public void GetVPKVersion()
        {
            var vpk = Format.VPK.Open(BasePortalPath + "portal_pak_dir.vpk");
            Assert.IsNotNull(vpk);
            Assert.AreEqual(vpk.Version, 2u);
        }

        private static string BasePortalPath
        {
            get
            {
                if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
                {
                    return @"D:\SteamLibrary\steamapps\common\Portal\portal\";
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
