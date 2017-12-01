--YIELD a declared variable and update it through invocations, testing the coroutine
CREATE FUNCTION factory01() RETURNS INT BEGIN
    DECLARE a INT;
    SET a = 1;
    YIELD a;
    SET a = a + 1;
    YIELD a;
    SET a = a + 1;
    YIELD a;
END;

SELECT factory01();
SELECT factory01();
SELECT factory01();
SELECT factory01(); --error

DROP FUNCTION factory01;