USE ARALGISver0;

BULK INSERT dbo.Vehicle
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-vehicle.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-vehicle-log.csv'
);

GO
