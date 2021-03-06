/*
 * Copyright 2016, Dario Casalinuovo
 * Distributed under the terms of the MIT License.
 */


#include "HTTPMediaIO.h"

#include <Handler.h>
#include <UrlProtocolRoster.h>

#include <stdio.h>


class FileListener : public BUrlProtocolAsynchronousListener {
public:
		FileListener(BAdapterIO* owner)
			:
			BUrlProtocolAsynchronousListener(true),
			fRequest(NULL),
			fAdapterIO(owner),
			fTotalSize(0)
		{
			fInputAdapter = fAdapterIO->BuildInputAdapter();
		}

		virtual ~FileListener() {};

		bool ConnectionSuccessful() const
		{
			return fRequest != NULL;
		}

		void ConnectionOpened(BUrlRequest* request)
		{
			if (fRequest != NULL)
				fRequest->Stop();

			fRequest = request;
			fTotalSize = request->Result().Length();
		}

		void DataReceived(BUrlRequest* request, const char* data,
			off_t position, ssize_t size)
		{
			if (request != fRequest)
				delete request;

			fInputAdapter->Write(data, size);
		}

		void RequestCompleted(BUrlRequest* request, bool success)
		{
			if (request != fRequest)
				return;

			fRequest = NULL;
			delete request;
		}

		off_t TotalSize() const
		{
			return fTotalSize;
		}

private:
		BUrlRequest*	fRequest;
		BAdapterIO*		fAdapterIO;
		BInputAdapter*	fInputAdapter;
		off_t			fTotalSize;
};


HTTPMediaIO::HTTPMediaIO(BUrl url)
	:
	BAdapterIO(B_MEDIA_STREAMING | B_MEDIA_SEEK_BACKWARD, B_INFINITE_TIMEOUT),
	fContext(NULL),
	fReq(NULL),
	fListener(NULL),
	fUrl(url)
{
}


HTTPMediaIO::~HTTPMediaIO()
{
}


ssize_t
HTTPMediaIO::WriteAt(off_t position, const void* buffer, size_t size)
{
	return B_NOT_SUPPORTED;
}


status_t
HTTPMediaIO::SetSize(off_t size)
{
	return B_NOT_SUPPORTED;
}


status_t
HTTPMediaIO::GetSize(off_t* size) const
{
	*size = fListener->TotalSize();
	return B_OK;
}


status_t
HTTPMediaIO::Open()
{
	fContext = new BUrlContext();
	fContext->AcquireReference();

	fListener = new FileListener(this);

	fReq = BUrlProtocolRoster::MakeRequest(fUrl,
		fListener, fContext);

	if (fReq == NULL)
		return B_ERROR;

	if (fReq->Run() < 0)
		return B_ERROR;

	return BAdapterIO::Open();
}


void
HTTPMediaIO::Close()
{
	delete fReq;
	delete fListener;

	fContext->ReleaseReference();
	delete fContext;

	BAdapterIO::Close();
}


status_t
HTTPMediaIO::SeekRequested(off_t position)
{
	return BAdapterIO::SeekRequested(position);
}
