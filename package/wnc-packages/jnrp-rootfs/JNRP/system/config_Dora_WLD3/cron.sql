CREATE TABLE crontab
(
    unit        integer NOT NULL,
    interval    integer,
    minute      integer,
    hour        integer,
    dayOfMonth  integer,
    month       integer,
    dayOfWeek   integer,
    command     text
);
insert into dbUpdateRegisterTbl values ("UMI_COMP_CRON",1,1,"crontab", 0,1,1,1,0);
insert into saveTables values("crontab");
