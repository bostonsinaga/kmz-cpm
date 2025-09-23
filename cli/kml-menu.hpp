void initKMLMenu(cm::Program<cm::Boolean> *cm_kmzcpm) {

  /** KML */

  cm::Word *cm_kml = cm_kmzcpm->createWord(
    "kml",
    "KML tools collection"
  );

  cm::Preset::applyFileIn(cm_kml, false);
  cm::Preset::applyFileOutOptional(cm_kml);

  /** CROP */

  cm::Boolean *cm_crop = cm_kml->createBoolean(
    "crop",
    "Crop placemarks inside a polygon",
    [](cm::Command *node)->cm::COMMAND_CALLBACK_CODE {
      std::cout << "> hello '" + node->getKeyword() + "'\n";
      return cm::COMMAND_CALLBACK_SUCCEEDED;
    }
  );

  cm_crop->makeRequired();
  cm::Preset::applyFileIn(cm_crop, false);
  cm::Preset::applyFileOutOptional(cm_crop);
}
