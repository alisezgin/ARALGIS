
USE [ARALGISver0]
GO

/****** Object:  Table [dbo].[BlackList]    Script Date: 12.05.2017 16:08:25 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[BlackList](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[LicensePlate] [nvarchar](8) NOT NULL,
	[InclusionDate] [date] NULL,
	[ExclusionDate] [date] NULL,
	[Explanation] [nvarchar](max) NULL,
 CONSTRAINT [PK_BlackList] PRIMARY KEY CLUSTERED 
(
	[LicensePlate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO

/****** Object:  Table [dbo].[Driver]    Script Date: 12.05.2017 16:09:21 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[Driver](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Name] [nvarchar](20) NULL,
	[LastName] [nvarchar](30) NULL,
	[Type] [int] NULL,
	[DriverLicenseIssueCity] [nvarchar](30) NULL,
	[DriverLicenseIssueDistrict] [nvarchar](30) NULL,
	[NatIDNo] [nvarchar](15) NULL,
	[DateofBirth] [date] NULL,
	[BirthPlace] [nvarchar](30) NULL,
	[NatIDIssueCity] [nvarchar](30) NULL,
	[NatIDIssueDistrict] [nvarchar](30) NULL,
	[NatIDVolNo] [int] NULL,
	[NatIDRowNo] [int] NULL,
	[NatIDLogNo] [int] NULL,
	[FatherName] [nvarchar](30) NULL,
	[MotherName] [nvarchar](30) NULL,
	[BloodType] [nvarchar](4) NULL,
	[IssueAuthority] [nvarchar](50) NULL,
 CONSTRAINT [PK_Driver] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO


/****** Object:  Table [dbo].[DriverType]    Script Date: 12.05.2017 16:10:11 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[DriverType](
	[DTID] [int] IDENTITY(1,1) NOT NULL,
	[Type] [nvarchar](30) NULL,
 CONSTRAINT [PK_DriverType] PRIMARY KEY CLUSTERED 
(
	[DTID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO


/****** Object:  Table [dbo].[Gate]    Script Date: 12.05.2017 16:11:01 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[Gate](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Type] [nvarchar](50) NULL,
	[PCName] [nvarchar](50) NULL,
	[IP] [nvarchar](50) NULL,
	[Notes] [nvarchar](max) NULL,
 CONSTRAINT [PK_Gate] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY] TEXTIMAGE_ON [PRIMARY]

GO


/****** Object:  Table [dbo].[User]    Script Date: 12.05.2017 16:11:47 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[User](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Name] [nvarchar](50) NULL,
	[LastName] [nvarchar](50) NULL,
	[Type] [int] NULL,
	[Nickname] [nvarchar](20) NULL,
	[RegistrationDateTime] [datetime] NULL,
	[Password] [nvarchar](20) NULL,
	[OldPassword] [nvarchar](20) NULL,
	[IsActive] [bit] NULL,
	[ExpungeDateTime] [datetime] NULL,
 CONSTRAINT [PK_User] PRIMARY KEY CLUSTERED 
(
	[ID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO


/****** Object:  Table [dbo].[UserLog]    Script Date: 12.05.2017 16:13:09 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[UserLog](
	[UserID] [int] IDENTITY(1,1) NOT NULL,
	[Nickname] [nvarchar](20) NOT NULL,
	[LoginDateTime] [datetime] NULL,
	[LogoffDateTime] [datetime] NULL,
 CONSTRAINT [PK_UserLog] PRIMARY KEY CLUSTERED 
(
	[UserID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO


/****** Object:  Table [dbo].[UserType]    Script Date: 12.05.2017 16:13:42 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[UserType](
	[UTID] [int] IDENTITY(1,1) NOT NULL,
	[Type] [nvarchar](20) NULL,
 CONSTRAINT [PK_UserType] PRIMARY KEY CLUSTERED 
(
	[UTID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

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


/****** Object:  Table [dbo].[VehicleType]    Script Date: 12.05.2017 16:15:33 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[VehicleType](
	[VTID] [int] IDENTITY(1,1) NOT NULL,
	[Type] [nvarchar](30) NULL,
 CONSTRAINT [PK_VehicleType] PRIMARY KEY CLUSTERED 
(
	[VTID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO

