
USE ARALGISver0;

BULK INSERT dbo.[VehicleType]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-vehicletype.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-vehicletype-log.csv'
);

GO
