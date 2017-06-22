USE ARALGISver0;

BULK INSERT dbo.BlackList
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-blacklist.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-blacklist-log.csv'
);

GO
