USE [ARALGISver0]
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

