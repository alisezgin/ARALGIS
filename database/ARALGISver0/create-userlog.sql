USE [ARALGISver0]
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

