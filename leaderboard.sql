CREATE DATABASE IF NOT EXISTS `leaderboard`;
USE `leaderboard`;
CREATE TABLE `leaderboard` (
	`player_name` TEXT NULL DEFAULT NULL COLLATE 'utf8mb4_uca1400_ai_ci',
	`score` INT(32) UNSIGNED NULL DEFAULT NULL,
	UNIQUE INDEX `name` (`player_name`) USING HASH
)
COLLATE='utf8mb4_uca1400_ai_ci'
ENGINE=InnoDB
;
