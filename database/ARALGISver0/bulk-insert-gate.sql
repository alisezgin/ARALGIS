USE ARALGISver0;

BULK INSERT dbo.Gate
FROM 'C:\ali\github-home\database\ARALGISver0\bulk-gate.csv'
WITH (
ROWTERMINATOR = '\n',
FIELDTERMINATOR = ',',
FIRSTROW = 2,
ERRORFILE = 'C:\ali\github-home\database\ARALGISver0\bulk-gate-log.csv'
);

GO
