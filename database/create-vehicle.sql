USE [AliDummy]
GO

/****** Object:  Table [dbo].[Vehicle]    Script Date: 4/25/2017 3:51:05 PM ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[Vehicle](
	[VehicleID] [bigint] IDENTITY(1,1) NOT NULL,
	[LicensePlate] [nchar](15) NULL,
	[LicensePlateImage] [nvarchar](max) NULL,
	[FrontViewImage] [nvarchar](max) NULL,
	[ChassisBottomImageCurrent] [nvarchar](max) NULL,
	[ChassisBottomImageRef] [nvarchar](max) NULL,
	[MostRecentVisitDate] [datetime] NULL,
	[DriverName] [nvarchar](max) NULL,
 CONSTRAINT [PK_Vehicle] PRIMARY KEY CLUSTERED 
(
	[VehicleID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO


