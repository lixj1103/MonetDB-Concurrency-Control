START TRANSACTION;

CREATE TABLE "test_hugeint" (
	"cHUGEINT"		HUGEINT
);

INSERT INTO "test_hugeint" ("cHUGEINT") VALUES (-9223372036854775807);
INSERT INTO "test_hugeint" ("cHUGEINT") VALUES (-9223372036854775808);
INSERT INTO "test_hugeint" ("cHUGEINT") VALUES (-9223372036854775809);

SELECT * FROM "test_hugeint";

ROLLBACK;