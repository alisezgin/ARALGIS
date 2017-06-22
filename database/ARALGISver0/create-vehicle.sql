USE [ARALGISver0]
GO

/****** Object:  Table [dbo].[Vehicle]    Script Date: 12.05.2017 16:04:52 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[Vehicle](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[LicensePlate] [nchar](8) NULL,
	[Type] [int] NULL,
	[RegistrationName] [nvarchar](max) NULL,
	[RegistrationDate] [date] NULL,
	[RegistrationNumber] [nvarchar](20) NULL,
	[LicensePlateImageFile] [nvarchar](max) NULL,
	[FrontViewImageFile] [nvarchar](max) NULL,
	[ChassisBottomReferenceImageFile] [nvarchar](max) NULL,
	[ChassisBottomLastImageFile] [nvarchar](max) NULL,
 CONSTRAINT [PK_Vehicle] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO

