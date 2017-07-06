
USE ARALGISver0;

BULK INSERT dbo.[VehiclePassage]
FROM 'C:\ali\github-home\ARALGIS\database\ARALGISver0\bulk-vehiclepassage-random.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\ARALGIS\database\ARALGISver0\bulk-vehiclepassage-random-log.csv'
);

GO
