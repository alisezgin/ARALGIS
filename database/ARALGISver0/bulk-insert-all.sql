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

BULK INSERT dbo.Driver
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-driver.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-driver-log.csv'
);

GO

BULK INSERT dbo.DriverType
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-drivertype.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-drivertype-log.csv'
);

GO

BULK INSERT dbo.Gate
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-gate.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-gate-log.csv'
);

GO

BULK INSERT dbo.[User]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-user.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-user-log.csv'
);

GO

BULK INSERT dbo.[UserLog]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-userlog.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-userlog-log.csv'
);

GO

BULK INSERT dbo.[UserType]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-usertype.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-usertype-log.csv'
);

GO

BULK INSERT dbo.Vehicle
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-vehicle.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-vehicle-log.csv'
);

GO

BULK INSERT dbo.[VehiclePassage]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-vehiclepassage.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-vehiclepassage-log.csv'
);

GO

BULK INSERT dbo.[VehicleType]
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-vehicletype.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-vehicletype-log.csv'
);

GO
