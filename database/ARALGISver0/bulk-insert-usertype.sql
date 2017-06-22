
USE ARALGISver0;

BULK INSERT dbo.[UserType]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-usertype.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-usertype-log.csv'
);

GO
