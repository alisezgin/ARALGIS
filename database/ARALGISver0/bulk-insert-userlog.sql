
USE ARALGISver0;

BULK INSERT dbo.[UserLog]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-userlog.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-userlog-log.csv'
);

GO
