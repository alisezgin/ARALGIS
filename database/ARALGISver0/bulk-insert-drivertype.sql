USE ARALGISver0;

BULK INSERT dbo.DriverType
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-drivertype.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-drivertype-log.csv'
);

GO
