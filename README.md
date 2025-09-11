# KMZ-CPM v2.0.0 patch 2025-06-09
*Departure from:*
*Geoton v1.1.4 2025-06-09*
*KML-TOWN v1.1.3 2023-07-08*
*KML-TOWN v0.10.4 ALPHA 2023-03-22*

A tool to analyze kmz/kml data of Google Earth.
Generate tree from text file, string, csv, or kmz/kml.

**WHAT THINGS YOU CAN DO WITH THIS ARE:**</br>
-- **convert** into *'.kml'* from *'.txt'* exported **WhatsApp** chat of share locations with their date</br>
-- **convert** *'.kml'* into *'.csv'* with folders as *columns* and data as *rows*</br>
-- **manipulate** *'.kml'* such as:</br>
------ *unite* pins into path</br>
------ *crop* placemarks</br>
------ *sort* placemarks</br>
------ *check* twin of placemarks</br>
------ *measure* paths distance</br>
------ *classify* placemarks by their similar style</br>
------ *folder* placemarks by their date<br>
------ *date* placemarks by their folder's date</br>
------ *filter* placemarks by text from their name or description</br>
------ *remove* paths by maximum/minimum distance

**HOW TO INSTALL:**</br>
-- compile the source code, by run *'./compile.[sh, bat]'* in CLI</br>
---- or <a href="https://github.com/bostonsinaga/geoton/releases/">download</a> released alpha version instead</br>
-- then add compiled program folder path (the 'bin/' folder where *'geoton'* program located)</br>
---- to 'path' in your 'Environment Variable'</br>
---- (if you don't want to do so, you have to always *'cd'* into the program folder every time you using it)</br>
-- and finally run *'geoton'* in your CLI</br>
---- (type *'geoton --help'* to start)</br>

I use this software in my previous job, as a coordinate data entry using **Google Earth**.</br>
This really help me. And I hope this can help you too. Thank you.</br>

**Boston Sinaga**


# COMMANDS

*******************
**INPUT MANDATORY**
*******************

-- TOGGLES --

MAIN_MENU_FLAG = "menu"<br>
HELP_MENU_FLAG = "help"<br>
VERSION_NUMBER_FLAG = "version"<br>
CONVERT_TXT_KML_FLAG = "convert"<br>
CONVERT_KML_CSV_FLAG = "convert"<br>
KML_CROP_FLAG = "kml"<br>
KML_SORT_FLAG = "kml"<br>
KML_PINS_PATH_FLAG = "kml"<br>
KML_TWINS_CHECK_FLAG = "kml"<br>
KML_TIDY_UP_STYLES = "kml"<br>
KML_MEASURE_PATHS_FLAG = "kml"<br>
KML_CLASSIFY_FLAG = "kml"<br>
KML_FOLDER_BY_DATE_FLAG = "kml"<br>
KML_DATE_BY_FOLDER_FLAG = "kml"<br>
KML_FILTER_STRING_FLAG = "kml"<br>
KML_REMOVE_PATHS_FLAG = "kml"<br>
CSV_CHANGE_SEPARATOR_FLAG = "csv"
TXT_COMPLETE_DATE_FLAG = "txt"

-- PARAMETERS --

CONVERT_TXT_KML_FLAG = "txt-in", "kml-out", "type"<br>
CONVERT_KML_CSV_FLAG = "kml-in", "csv-out"<br>
KML_CROP_FLAG = "crop", "start-point", "end-point", "type"<br>
KML_SORT_FLAG = "sort", "start-point", "end-point", "type"<br>
KML_PINS_PATH_FLAG = "pins-path", "start-point", "end-point"<br>
KML_TWINS_CHECK_FLAG = "twins-check", "type", "radius"<br>
KML_TIDY_UP_STYLES = "tidy-up-styles"<br>
KML_MEASURE_PATHS_FLAG = "measure-paths"<br>
KML_CLASSIFY_FLAG = "classify"<br>
KML_FOLDER_BY_DATE_FLAG = "folder-by-date"<br>
KML_DATE_BY_FOLDER_FLAG = "date-by-folder"<br>
KML_FILTER_STRING_FLAG = "filter-string", "text"<br>
KML_REMOVE_PATHS_FLAG = "remove-paths"<br>
CSV_CHANGE_SEPARATOR_FLAG = "change-separator", "old-sign", "new-sign"
TXT_COMPLETE_DATE_FLAG = "complete-date"

******************
**INPUT OPTIONAL**
******************

-- TOGGLES --

CONVERT_TXT_KML_FLAG = "sort"<br>
KML_CROP_FLAG = "include-folders"<br>
KML_SORT_FLAG = "include-folders"<br>
KML_MEASURE_PATHS_FLAG = "info"<br>
KML_TWINS_CHECK_FLAG = "include-folders", "only-similar-style", "path-text-prioritize-first"<br>
KML_CLASSIFY_FLAG = "clean-folders", "include-folders"<br>
KML_DATE_BY_FOLDER_FLAG = "override-dated"

-- PARAMETERS --

KML_CROP_FLAG = "out"<br>
KML_SORT_FLAG = "out"<br>
KML_PINS_PATH_FLAG = "out"<br>
KML_TWINS_CHECK_FLAG = "out"<br>
KML_TIDY_UP_STYLES = "out"<br>
KML_MEASURE_PATHS_FLAG = "out"<br>
KML_CLASSIFY_FLAG = "out"<br>
KML_FOLDER_BY_DATE_FLAG = "out"<br>
KML_DATE_BY_FOLDER_FLAG = "out"<br>
KML_FILTER_STRING_FLAG = "out"<br>
KML_REMOVE_PATHS_FLAG = "out", "over-distance", "under-distance"<br>
CSV_CHANGE_SEPARATOR_FLAG = "out"
TXT_COMPLETE_DATE_FLAG = "out"


# SCREENSHOTS

**CROP PINS**

<img src="https://raw.githubusercontent.com/bostonsinaga/geoton/main/test/screenshots/scrsht-ge-wide-pins.jpg" alt="scrsht-ge-wide-pins.jpg" width="800"/>
<img src="https://raw.githubusercontent.com/bostonsinaga/geoton/main/test/screenshots/scrsht-cli-crop-pins.png" alt="scrsht-cli-crop-pins.png" width="800"/>
<img src="https://raw.githubusercontent.com/bostonsinaga/geoton/main/test/screenshots/scrsht-ge-selected-pins.jpg" alt="scrsht-ge-selected-pins.jpg" width="800"/>
<img src="https://raw.githubusercontent.com/bostonsinaga/geoton/main/test/screenshots/scrsht-ge-cropped-pins.jpg" alt="scrsht-ge-cropped-pins.jpg" width="800"/>

**PINS TO PATH**

<img src="https://raw.githubusercontent.com/bostonsinaga/geoton/main/test/screenshots/scrsht-cli-pins-path.png" alt="scrsht-cli-pins-path.png" width="800"/>
<img src="https://raw.githubusercontent.com/bostonsinaga/geoton/main/test/screenshots/scrsht-ge-path-of-pins.jpg" alt="scrsht-ge-path-of-pins.jpg" width="800"/>

**KML TO SHEET**

<img src="https://raw.githubusercontent.com/bostonsinaga/geoton/main/test/screenshots/scrsht-cli-kml-csv.png" alt="scrsht-cli-kml-csv.png" width="800"/>
<img src="https://raw.githubusercontent.com/bostonsinaga/geoton/main/test/screenshots/scrsht-sheet.jpg" alt="scrsht-sheet.jpg" width="800"/>
