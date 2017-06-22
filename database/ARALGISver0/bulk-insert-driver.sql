USE ARALGISver0;

BULK INSERT dbo.Driver
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-driver.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-driver-log.csv'
);

GO
