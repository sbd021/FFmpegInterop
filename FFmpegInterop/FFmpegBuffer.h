//*****************************************************************************
//
//	Copyright 2015 Microsoft Corporation
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//	http ://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.
//
//*****************************************************************************

#pragma once

namespace winrt::FFmpegInterop::implementation
{
	class FFmpegBuffer :
		public implements<
			FFmpegBuffer, 
			Windows::Storage::Streams::IBuffer,
			::Windows::Storage::Streams::IBufferByteAccess>
	{
	public:
		FFmpegBuffer(_In_ AVBufferRef* bufferRef) noexcept :
			m_length(bufferRef->size),
			m_buffer(reinterpret_cast<uint8_t*>(bufferRef->data), [ AVBufferRef_ptr{ av_buffer_ref(bufferRef) } ](uint8_t*) noexcept { })
		{

		}

		FFmpegBuffer(_In_ const AVPacket* packet) noexcept :
			FFmpegBuffer(packet->buf)
		{

		}

		FFmpegBuffer(_In_ AVBufferRef_ptr bufferRef) noexcept :
			m_length(bufferRef->size),
			m_buffer(reinterpret_cast<uint8_t*>(bufferRef->data), [ AVBufferRef_ptr{ bufferRef.get() } ](uint8_t*) noexcept { })
		{
			bufferRef.release(); // The lambda has taken ownership
		}

		FFmpegBuffer(AVBlob_ptr buffer, uint32_t length) noexcept :
			m_length(length),
			m_buffer(static_cast<uint8_t*>(buffer.release()))
		{

		}

		// IBuffer
		uint32_t Capacity() const noexcept
		{
			return m_length;
		}

		uint32_t Length() const noexcept
		{
			return m_length;
		}

		void Length(_In_ uint32_t length)
		{
			// We have a fixed length
			THROW_HR(CO_E_NOT_SUPPORTED);
		}

		// IBufferByteAccess
		STDMETHODIMP Buffer(_Outptr_ uint8_t** buffer) noexcept override
		{
			RETURN_HR_IF_NULL(E_POINTER, buffer);
			*buffer = m_buffer.get();
			return S_OK;
		}

	private:
		uint32_t m_length;
		std::unique_ptr<uint8_t, std::function<void(uint8_t*)>> m_buffer;
	};
}
