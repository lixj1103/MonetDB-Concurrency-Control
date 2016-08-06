-- use accumulated aggregation
set schema iot;
set optimizer='iot_pipe';

create stream table stmp10 (t timestamp, sensor integer, val decimal(8,2)) ;
call iot.window('iot','stmp10',2);

create table tmp_aggregate(tmp_total decimal(8,2), tmp_count decimal(8,2));
insert into tmp_aggregate values(0.0,0.0);

create procedure collector()
begin
	update tmp_aggregate
		set tmp_total = tmp_total + (select sum(val) from iot.stmp10),
			tmp_count = tmp_count + (select count(*) from iot.stmp10);
	delete from iot.stmp10;
end;

insert into stmp10 values('2005-09-23 12:34:26.000',1,9.0);
insert into stmp10 values('2005-09-23 12:34:27.000',1,11.0);
insert into stmp10 values('2005-09-23 12:34:28.000',1,13.0);
insert into stmp10 values('2005-09-23 12:34:28.000',1,15.0);
select * from stmp10;

call iot.query('iot','collector');

--select * from iot.baskets();
--select * from iot.queries();

call iot.resume();
-- wait a few seconds
call iot.wait(5000);

select * from tmp_aggregate;

call iot.stop();
select * from iot.errors();
drop procedure collector;
drop table stmp10;
drop table tmp_aggregate;