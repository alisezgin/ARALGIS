USE [ARALGISver0]
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

