
USE ARALGISver0;

BULK INSERT dbo.[VehiclePassage]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-vehiclepassage.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-vehiclepassage-log.csv'
);

GO
