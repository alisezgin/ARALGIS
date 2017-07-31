USE [ARALGISver0]
GO

/****** Object:  Table [dbo].[VehiclePassage]    Script Date: 13.05.2017 16:37:12 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[VehiclePassage](
	[VehicleID] [int] IDENTITY(1,1) NOT NULL,
	[LicensePlate] [nvarchar](8) NOT NULL,
	[EntryDateTime] [datetime] NULL,
	[ExitDateTime] [datetime] NULL,
	[UserID] [int] NULL,
	[GateID] [int] NULL,
	[DriverID] [int] NULL,
	[PermissionGranted] [bit] NULL,
 CONSTRAINT [PK_VehiclePassage_1] PRIMARY KEY CLUSTERED 
(
	[VehicleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO
