USE ARALGISver0;

BULK INSERT dbo.[User]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-user.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-user-log.csv'
);

GO
