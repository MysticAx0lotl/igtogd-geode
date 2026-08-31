# v1.0.5
- 2.2081 support (Geode v5)
- Rewrite the import logic, should be significantly faster
- Change file picker to only allow .lvl files (except on iOS; where it's .dat due to Geode specific issues regarding the folder picker)
- Name imported levels after the .lvl file (falls back to "Impossible Game Import" if the file name is empty or if it's a .dat file)

# v1.0.4
 * Fix memory management issues preventing the mod from working on Windows
 * Temporarily remove iOS builds until a solution for the folder picker crash is found

# v1.0.3
 * Fix _massive_ oversights with the positioning of certain triggers

# v1.0.2
 * Add input validation

# v1.0.1
 * Cross-platform support courtesy of @slideglide

# v1.0.0
 * Initial release!
