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

