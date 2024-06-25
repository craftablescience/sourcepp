## Cache Generation Steps

1. Open the following URL in your browser:
   - Source:
     - https://steamdb.info/tech/Engine/Source/?sort=name_asc
   - Source 2:
     - https://steamdb.info/tech/Engine/Source2/?sort=name_asc
2. Change "`100` entries per page" to "`All (slow)` entries per page"
3. Run the following code in the console:
   ```js
   let o = "";
   for (const e of document.getElementById("DataTables_Table_0").children[2].children) {
       o += e.getAttribute("data-appid") + ", // " + e.children[2]. children[0].innerText + "\n";
   }
   console.log(o);
   ```
4. Copy the given output and paste it into the relevant file.
5. Trim entries manually which don't actually match the given engine. SteamDB isn't perfect and stuff slips through the system. (For example, as of May 26, 2024, Portal 2: Community Edition is listed as a game that uses Source 2.)
