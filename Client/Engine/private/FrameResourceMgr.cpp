#include "FrameResourceMgr.h"

IMPLEMENT_SINGLETON(CFrameResourceMgr);

CFrameResourceMgr::CFrameResourceMgr()
{
}

void CFrameResourceMgr::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* pCommandList, ID3D12CommandQueue* pCommandQueue, UINT frameCount)
{
	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_MainFence))))
		MSG_BOX("Failed to Create : Fence");

	m_MainFenceValue = 0;

	m_CurrentFrameIndex = 0;

	m_NumFrames = frameCount;
	m_FrameResources.resize(frameCount);

	for (int i = 0; i < m_NumFrames; ++i)
	{
		m_FrameResources[i] = CFrameResource::Create(device);
		if (m_FrameResources[i] == nullptr)
			return;
	}

	m_CommandList = pCommandList;
	m_CommandQueue = pCommandQueue;

	Safe_AddRef(m_CommandList);
	Safe_AddRef(m_CommandQueue);
}

void CFrameResourceMgr::BeginFrame()
{
	m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_NumFrames;

	auto& frame = m_FrameResources[m_CurrentFrameIndex];

	// GPU�� �� FrameResource�� ���� ��� ���̸� ��ٸ�
	if (m_MainFence->GetCompletedValue() < frame->Get_Fence_Value())
	{
		HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		m_MainFence->SetEventOnCompletion(frame->Get_Fence_Value(), eventHandle);
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void CFrameResourceMgr::Reset_CommandList_and_Allocator(ID3D12PipelineState* PSO)
{
	auto& frame = m_FrameResources[m_CurrentFrameIndex];

	frame->Get_Current_CmdListAlloc()->Reset();

	m_CommandList->Reset(frame->Get_Current_CmdListAlloc(), PSO);
}

void CFrameResourceMgr::SignalAndAdvance()
{
	++m_MainFenceValue;
	m_CommandQueue->Signal(m_MainFence, m_MainFenceValue);
	m_FrameResources[m_CurrentFrameIndex]->Set_Fence_Value(m_MainFenceValue);
}

void CFrameResourceMgr::Flush_CommandQueue()
{
	m_MainFenceValue++;

	// Ŀ�ǵ���� ó���� GPU���� ����Ǳ� ������ ���� Ŀ�ǵ���� ó��������� CPU���� �˱� ����ϴ�.
	// �׷��Ƿ� ��� Ŀ�ǵ尡 ó������ �� �� �潺 ������ �����ϴ� �ν�Ʈ������ Ŀ�ǵ� ť�� �߰��մϴ�.
	// Signal()�� ȣ���ϱ� ���� ������ Ŀ�ǵ���� ó���Ǳ� ������ �� �潺 ������ �������� �ʽ��ϴ�.
	if (FAILED(m_CommandQueue->Signal(m_MainFence, m_MainFenceValue)))
		MSG_BOX("Failed to Signal : Fence");

	// GPU�� �� �潺 ���������� ��ɵ��� �Ϸ��� ������ ��ٸ��ϴ�.
	if (m_MainFence->GetCompletedValue() < m_MainFenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

		// GPU�� �� �潺 ������ ���������� �̺�Ʈ�� �ߵ���ŵ�ϴ�.
		if (FAILED(m_MainFence->SetEventOnCompletion(m_MainFenceValue, eventHandle)))
			MSG_BOX("Failed to Create : Event");

		// GPU�� �� �潺�� �����ϰ� �̺�Ʈ�� �ߵ��ɶ����� ��ٸ��ϴ�.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}



ID3D12CommandAllocator* CFrameResourceMgr::Get_CurrentAllocator() const
{
	return m_FrameResources[m_CurrentFrameIndex]->Get_Current_CmdListAlloc();
}

CFrameResource* CFrameResourceMgr::Get_Current_FrameResource()
{
	return m_FrameResources[m_CurrentFrameIndex];
}

void CFrameResourceMgr::Free()
{
	for (auto& frameResource : m_FrameResources)
		Safe_Release(frameResource);
	Safe_Release(m_MainFence);
	Safe_Release(m_CommandList);
	Safe_Release(m_CommandQueue);
}
