USE AliDummy;

BULK INSERT dbo.Vehicle
FROM 'O:\ali\github-home\database\bulk-vehicle.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'O:\ali\github-home\database\bulk-vehicle-log.csv'
);

GO
