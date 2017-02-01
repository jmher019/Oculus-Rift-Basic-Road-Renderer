#include "..\include\TextureBuffer.h"

TextureBuffer::TextureBuffer(ovrSession session, bool renderTarget, bool displayableOnHmd, OVR::Sizei size, int mipLevels, unsigned char * data, int sampleCount) :
	session(session),
	textureChain(nullptr),
	texId(0),
	fboId(0),
	texSize(0, 0) {
	UNREFERENCED_PARAMETER(sampleCount);

	assert(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

	texSize = size;

	if (displayableOnHmd)
	{
		// This texture isn't necessarily going to be a rendertarget, but it usually is.
		assert(session); // No HMD? A little odd.
		assert(sampleCount == 1); // ovr_CreateSwapTextureSetD3D11 doesn't support MSAA.

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Width = size.w;
		desc.Height = size.h;
		desc.MipLevels = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleCount = 1;
		desc.StaticImage = ovrFalse;

		ovrResult result = ovr_CreateTextureSwapChainGL(this->session, &desc, &this->textureChain);

		int length = 0;
		ovr_GetTextureSwapChainLength(session, this->textureChain, &length);

		if (OVR_SUCCESS(result))
		{
			for (int i = 0; i < length; ++i)
			{
				GLuint chainTexId;
				ovr_GetTextureSwapChainBufferGL(this->session, this->textureChain, i, &chainTexId);
				glBindTexture(GL_TEXTURE_2D, chainTexId);

				if (renderTarget)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				}
			}
		}
	}
	else
	{
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);

		if (renderTarget)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	if (mipLevels > 1)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glGenFramebuffers(1, &fboId);
}

TextureBuffer::~TextureBuffer(void) {
	if (textureChain)
	{
		ovr_DestroyTextureSwapChain(session, textureChain);
		textureChain = nullptr;
	}
	if (texId)
	{
		glDeleteTextures(1, &texId);
		texId = 0;
	}
	if (fboId)
	{
		glDeleteFramebuffers(1, &fboId);
		fboId = 0;
	}
}

void TextureBuffer::SetAndClearRenderSurface(DepthBuffer* dbuffer) {
	GLuint curTexId;
	if (textureChain)
	{
		int curIndex;
		ovr_GetTextureSwapChainCurrentIndex(session, textureChain, &curIndex);
		ovr_GetTextureSwapChainBufferGL(session, textureChain, curIndex, &curTexId);
	}
	else
	{
		curTexId = texId;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dbuffer->texId, 0);

	glViewport(0, 0, texSize.w, texSize.h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void TextureBuffer::UnsetRenderSurface(void) {
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
}

void TextureBuffer::Commit() {
	if (textureChain)
	{
		ovr_CommitTextureSwapChain(session, textureChain);
	}
}