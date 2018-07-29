#pragma once
#include "memory.h"
#include <cstdint>

#define EPT_PAGE_WALK_LENGTH_4                                       0x00000003

namespace ia32 {

struct ept_ptr_t
{
  union
  {
    uint64_t flags;

    struct
    {
      uint64_t memory_type : 3;
      uint64_t page_walk_length : 3;
      uint64_t enable_access_and_dirty_flags : 1;
      uint64_t reserved_1 : 5;
      uint64_t page_frame_number : 36;
    };
  };
};

struct ept_pml4e_t
{
  union
  {
    uint64_t flags;

    struct
    {
      uint64_t read_access : 1;
      uint64_t write_access : 1;
      uint64_t execute_access : 1;
      uint64_t reserved_1 : 5;
      uint64_t accessed : 1;
      uint64_t reserved_2 : 1;
      uint64_t user_mode_execute : 1;
      uint64_t reserved_3 : 1;
      uint64_t page_frame_number : 36;
    };
  };
};

struct ept_pdpte_1gb_t
{
  union
  {
    uint64_t flags;

    struct
    {
      uint64_t read_access : 1;
      uint64_t write_access : 1;
      uint64_t execute_access : 1;
      uint64_t memory_type : 3;
      uint64_t ignore_pat : 1;
      uint64_t large_page : 1;
      uint64_t accessed : 1;
      uint64_t dirty : 1;
      uint64_t user_mode_execute : 1;
      uint64_t reserved_1 : 19;
      uint64_t page_frame_number : 18;
      uint64_t reserved_2 : 15;
      uint64_t suppress_ve : 1;
    };
  };
};

struct ept_pdpte_t
{
  union
  {
    uint64_t flags;

    struct
    {
      uint64_t read_access : 1;
      uint64_t write_access : 1;
      uint64_t execute_access : 1;
      uint64_t reserved_1 : 5;
      uint64_t accessed : 1;
      uint64_t reserved_2 : 1;
      uint64_t user_mode_execute : 1;
      uint64_t reserved_3 : 1;
      uint64_t page_frame_number : 36;
    };
  };
};

struct ept_pde_2mb_t
{
  union
  {
    uint64_t flags;

    struct
    {
      uint64_t read_access : 1;
      uint64_t write_access : 1;
      uint64_t execute_access : 1;
      uint64_t memory_type : 3;
      uint64_t ignore_pat : 1;
      uint64_t large_page : 1;
      uint64_t accessed : 1;
      uint64_t dirty : 1;
      uint64_t user_mode_execute : 1;
      uint64_t reserved_1 : 10;
      uint64_t page_frame_number : 27;
      uint64_t reserved_2 : 15;
      uint64_t suppress_ve : 1;
    };
  };
};

struct ept_pde_t
{
  union
  {
    uint64_t flags;

    struct
    {
      uint64_t read_access : 1;
      uint64_t write_access : 1;
      uint64_t execute_access : 1;
      uint64_t reserved_1 : 5;
      uint64_t accessed : 1;
      uint64_t reserved_2 : 1;
      uint64_t user_mode_execute : 1;
      uint64_t reserved_3 : 1;
      uint64_t page_frame_number : 36;
    };
  };
};

struct ept_pte_t
{
  union
  {
    uint64_t flags;

    struct
    {
      uint64_t read_access : 1;
      uint64_t write_access : 1;
      uint64_t execute_access : 1;
      uint64_t memory_type : 3;
      uint64_t ignore_pat : 1;
      uint64_t reserved_1 : 1;
      uint64_t accessed : 1;
      uint64_t dirty : 1;
      uint64_t user_mode_execute : 1;
      uint64_t reserved_2 : 1;
      uint64_t page_frame_number : 36;
      uint64_t reserved_3 : 15;
      uint64_t suppress_ve : 1;
    };
  };
};

struct epte_t
{
  union
  {
    uint64_t        flags;

    ept_pml4e_t     pml4;
    ept_pdpte_1gb_t pdpt_1gb;
    ept_pdpte_t     pdpt;
    ept_pde_2mb_t   pd_2mb;
    ept_pde_t       pd;
    ept_pte_t       pt;

    struct
    {
      uint64_t read_access : 1;
      uint64_t write_access : 1;
      uint64_t execute_access : 1;
      uint64_t memory_type : 3;
      uint64_t ignore_pat : 1;
      uint64_t large_page : 1;
      uint64_t accessed : 1;
      uint64_t dirty : 1;
      uint64_t user_mode_execute : 1;
      uint64_t reserved_1 : 1;
      uint64_t page_frame_number : 36;
      uint64_t reserved_2 : 15;
      uint64_t suppress_ve : 1;
    };
  };

  void update(pa_t pa) noexcept
  {
    read_access = write_access = execute_access = true;
    page_frame_number = pa.pfn();
  }

  void update(pa_t pa, mtype type) noexcept
  {
    update(pa);
    memory_type = static_cast<uint64_t>(type);
  }

  void update(pa_t pa, mtype type, bool large) noexcept
  {
    update(pa, type);
    large_page = large;
  }

  epte_t* subtable() const noexcept
  {
    return is_present()
      ? reinterpret_cast<epte_t*>(pa_t::from_pfn(page_frame_number).va())
      : nullptr;
  }

  bool is_present() const noexcept
  {
    return read_access || write_access || execute_access;
  }
};

static_assert(sizeof(epte_t) == 8);

}
