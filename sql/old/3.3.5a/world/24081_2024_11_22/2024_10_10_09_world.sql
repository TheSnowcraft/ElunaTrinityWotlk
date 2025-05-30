-- Pathing for Tideress: 12759
SET @NPC=12759;
SET @GUID=34224;
SET @PATH=@GUID * 10;
UPDATE `creature` SET `position_x`=1958.41,`position_y`=-945.92,`position_z`=70.11,`orientation`=4.153880,`wander_distance`=0,`MovementType`=2 WHERE `guid`=@GUID;
UPDATE `creature_template` SET `MovementType`=2 WHERE `entry`=@NPC;
UPDATE `creature_template_addon` SET `PvpFlags`=0 WHERE `entry`=@NPC;
DELETE FROM `creature_addon` WHERE `guid`=@GUID;
INSERT INTO `creature_addon` (`guid`, `path_id`, `mount`, `MountCreatureID`, `StandState`, `AnimTier`, `VisFlags`, `SheathState`, `PvPFlags`, `emote`, `visibilityDistanceType`, `auras`) VALUES
(@GUID, @PATH, 0, 0, 0, 0, 0, 1, 0, 0, 0, NULL);
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`, `point`, `position_x`, `position_y`, `position_z`, `orientation`, `delay`, `move_type`, `action`, `action_chance`, `wpguid`) VALUES
(@PATH, 1, 1950.76, -957.199, 65.9789, NULL, 0, 0, 0, 100, 0),
(@PATH, 2, 1949.7, -971.117, 62.4051, NULL, 0, 0, 0, 100, 0),
(@PATH, 3, 1952.04, -984.871, 63.578, NULL, 0, 0, 0, 100, 0),
(@PATH, 4, 1958.59, -995.102, 69.6525, NULL, 0, 0, 0, 100, 0),
(@PATH, 5, 1958.25, -1010.11, 77.985, NULL, 0, 0, 0, 100, 0),
(@PATH, 6, 1965.77, -1023.12, 88.7347, NULL, 0, 0, 0, 100, 0),
(@PATH, 7, 1968.2, -1033.66, 93.133, NULL, 0, 0, 0, 100, 0),
(@PATH, 8, 1966.17, -1043.44, 94.9841, NULL, 0, 0, 0, 100, 0),
(@PATH, 9, 1973.94, -1049.95, 95.6832, NULL, 0, 0, 0, 100, 0),
(@PATH, 10, 1986.89, -1056.69, 96.4432, NULL, 0, 0, 0, 100, 0),
(@PATH, 11, 1995.48, -1071.04, 94.9886, NULL, 0, 0, 0, 100, 0),
(@PATH, 12, 1997.02, -1086.65, 94.846, NULL, 0, 0, 0, 100, 0),
(@PATH, 13, 2001.35, -1109.18, 95.674, NULL, 0, 0, 0, 100, 0),
(@PATH, 14, 1992.91, -1089.13, 94.9791, NULL, 0, 0, 0, 100, 0),
(@PATH, 15, 1977.37, -1091.98, 93.5929, NULL, 0, 0, 0, 100, 0),
(@PATH, 16, 1960.89, -1106.05, 81.5004, NULL, 0, 0, 0, 100, 0),
(@PATH, 17, 1950.73, -1121.58, 76.5911, NULL, 0, 0, 0, 100, 0),
(@PATH, 18, 1952.45, -1137.8, 74.7081, NULL, 0, 0, 0, 100, 0),
(@PATH, 19, 1951.68, -1152.21, 74.7775, NULL, 0, 0, 0, 100, 0),
(@PATH, 20, 1959.66, -1179.49, 63.8513, NULL, 0, 0, 0, 100, 0),
(@PATH, 21, 1967.89, -1196.3, 59.4363, NULL, 0, 0, 0, 100, 0),
(@PATH, 22, 1966.14, -1211.91, 57.2217, NULL, 0, 0, 0, 100, 0),
(@PATH, 23, 1941.16, -1212.98, 56.7257, NULL, 0, 0, 0, 100, 0),
(@PATH, 24, 1932.03, -1226.02, 59.3121, NULL, 0, 0, 0, 100, 0),
(@PATH, 25, 1940.4, -1246.86, 58.4144, NULL, 0, 0, 0, 100, 0),
(@PATH, 26, 1947.89, -1264.91, 60.367, NULL, 0, 0, 0, 100, 0),
(@PATH, 27, 1949.59, -1283.22, 65.9393, NULL, 0, 0, 0, 100, 0),
(@PATH, 28, 1941.54, -1297.67, 74.969, NULL, 0, 0, 0, 100, 0),
(@PATH, 29, 1936.7, -1307.34, 80.0226, NULL, 0, 0, 0, 100, 0),
(@PATH, 30, 1920.6, -1309.47, 82.8535, NULL, 0, 0, 0, 100, 0),
(@PATH, 31, 1902.6, -1303.77, 86.9758, NULL, 0, 0, 0, 100, 0),
(@PATH, 32, 1920.6, -1309.47, 82.8535, NULL, 0, 0, 0, 100, 0),
(@PATH, 33, 1936.7, -1307.34, 80.0226, NULL, 0, 0, 0, 100, 0),
(@PATH, 34, 1941.54, -1297.67, 74.969, NULL, 0, 0, 0, 100, 0),
(@PATH, 35, 1949.59, -1283.22, 65.9393, NULL, 0, 0, 0, 100, 0),
(@PATH, 36, 1947.89, -1264.91, 60.367, NULL, 0, 0, 0, 100, 0),
(@PATH, 37, 1940.4, -1246.86, 58.4144, NULL, 0, 0, 0, 100, 0),
(@PATH, 38, 1932.03, -1226.02, 59.3121, NULL, 0, 0, 0, 100, 0),
(@PATH, 39, 1941.14, -1212.98, 56.7217, NULL, 0, 0, 0, 100, 0),
(@PATH, 40, 1966.12, -1211.9, 57.2257, NULL, 0, 0, 0, 100, 0),
(@PATH, 41, 1967.89, -1196.3, 59.4363, NULL, 0, 0, 0, 100, 0),
(@PATH, 42, 1959.66, -1179.49, 63.8513, NULL, 0, 0, 0, 100, 0),
(@PATH, 43, 1951.68, -1152.21, 74.7775, NULL, 0, 0, 0, 100, 0),
(@PATH, 44, 1952.45, -1137.8, 74.7081, NULL, 0, 0, 0, 100, 0),
(@PATH, 45, 1950.73, -1121.58, 76.5911, NULL, 0, 0, 0, 100, 0),
(@PATH, 46, 1960.89, -1106.05, 81.5004, NULL, 0, 0, 0, 100, 0),
(@PATH, 47, 1977.37, -1091.98, 93.5929, NULL, 0, 0, 0, 100, 0),
(@PATH, 48, 1992.91, -1089.13, 94.9791, NULL, 0, 0, 0, 100, 0),
(@PATH, 49, 2001.35, -1109.18, 95.674, NULL, 0, 0, 0, 100, 0),
(@PATH, 50, 1997.02, -1086.65, 94.846, NULL, 0, 0, 0, 100, 0),
(@PATH, 51, 1995.48, -1071.04, 94.9886, NULL, 0, 0, 0, 100, 0),
(@PATH, 52, 1986.89, -1056.69, 96.4432, NULL, 0, 0, 0, 100, 0),
(@PATH, 53, 1973.94, -1049.95, 95.6832, NULL, 0, 0, 0, 100, 0),
(@PATH, 54, 1966.28, -1043.53, 94.9587, NULL, 0, 0, 0, 100, 0),
(@PATH, 55, 1968.19, -1033.7, 93.1832, NULL, 0, 0, 0, 100, 0),
(@PATH, 56, 1965.77, -1023.12, 88.7347, NULL, 0, 0, 0, 100, 0),
(@PATH, 57, 1958.25, -1010.11, 77.985, NULL, 0, 0, 0, 100, 0),
(@PATH, 58, 1958.59, -995.102, 69.6525, NULL, 0, 0, 0, 100, 0),
(@PATH, 59, 1952.04, -984.871, 63.578, NULL, 0, 0, 0, 100, 0),
(@PATH, 60, 1949.7, -971.117, 62.4051, NULL, 0, 0, 0, 100, 0),
(@PATH, 61, 1950.76, -957.199, 65.9789, NULL, 0, 0, 0, 100, 0),
(@PATH, 62, 1956.02, -946.128, 71.0499, NULL, 0, 0, 0, 100, 0);
