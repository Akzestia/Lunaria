// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: vpn_graph.proto

#include "vpn_graph.pb.h"

#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
#include "google/protobuf/generated_message_tctable_impl.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;

inline constexpr Vpn_graph::Impl_::Impl_(
    ::_pbi::ConstantInitialized) noexcept
      : id_{0},
        _cached_size_{0} {}

template <typename>
PROTOBUF_CONSTEXPR Vpn_graph::Vpn_graph(::_pbi::ConstantInitialized)
    : _impl_(::_pbi::ConstantInitialized()) {}
struct Vpn_graphDefaultTypeInternal {
  PROTOBUF_CONSTEXPR Vpn_graphDefaultTypeInternal() : _instance(::_pbi::ConstantInitialized{}) {}
  ~Vpn_graphDefaultTypeInternal() {}
  union {
    Vpn_graph _instance;
  };
};

PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT
    PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 Vpn_graphDefaultTypeInternal _Vpn_graph_default_instance_;
static ::_pb::Metadata file_level_metadata_vpn_5fgraph_2eproto[1];
static constexpr const ::_pb::EnumDescriptor**
    file_level_enum_descriptors_vpn_5fgraph_2eproto = nullptr;
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_vpn_5fgraph_2eproto = nullptr;
const ::uint32_t TableStruct_vpn_5fgraph_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(
    protodesc_cold) = {
    ~0u,  // no _has_bits_
    PROTOBUF_FIELD_OFFSET(::Vpn_graph, _internal_metadata_),
    ~0u,  // no _extensions_
    ~0u,  // no _oneof_case_
    ~0u,  // no _weak_field_map_
    ~0u,  // no _inlined_string_donated_
    ~0u,  // no _split_
    ~0u,  // no sizeof(Split)
    PROTOBUF_FIELD_OFFSET(::Vpn_graph, _impl_.id_),
};

static const ::_pbi::MigrationSchema
    schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
        {0, -1, -1, sizeof(::Vpn_graph)},
};

static const ::_pb::Message* const file_default_instances[] = {
    &::_Vpn_graph_default_instance_._instance,
};
const char descriptor_table_protodef_vpn_5fgraph_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n\017vpn_graph.proto\"\027\n\tVpn_graph\022\n\n\002id\030\001 \001"
    "(\005b\006proto3"
};
static ::absl::once_flag descriptor_table_vpn_5fgraph_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_vpn_5fgraph_2eproto = {
    false,
    false,
    50,
    descriptor_table_protodef_vpn_5fgraph_2eproto,
    "vpn_graph.proto",
    &descriptor_table_vpn_5fgraph_2eproto_once,
    nullptr,
    0,
    1,
    schemas,
    file_default_instances,
    TableStruct_vpn_5fgraph_2eproto::offsets,
    file_level_metadata_vpn_5fgraph_2eproto,
    file_level_enum_descriptors_vpn_5fgraph_2eproto,
    file_level_service_descriptors_vpn_5fgraph_2eproto,
};

// This function exists to be marked as weak.
// It can significantly speed up compilation by breaking up LLVM's SCC
// in the .pb.cc translation units. Large translation units see a
// reduction of more than 35% of walltime for optimized builds. Without
// the weak attribute all the messages in the file, including all the
// vtables and everything they use become part of the same SCC through
// a cycle like:
// GetMetadata -> descriptor table -> default instances ->
//   vtables -> GetMetadata
// By adding a weak function here we break the connection from the
// individual vtables back into the descriptor table.
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_vpn_5fgraph_2eproto_getter() {
  return &descriptor_table_vpn_5fgraph_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_vpn_5fgraph_2eproto(&descriptor_table_vpn_5fgraph_2eproto);
// ===================================================================

class Vpn_graph::_Internal {
 public:
};

Vpn_graph::Vpn_graph(::google::protobuf::Arena* arena)
    : ::google::protobuf::Message(arena) {
  SharedCtor(arena);
  // @@protoc_insertion_point(arena_constructor:Vpn_graph)
}
Vpn_graph::Vpn_graph(
    ::google::protobuf::Arena* arena, const Vpn_graph& from)
    : Vpn_graph(arena) {
  MergeFrom(from);
}
inline PROTOBUF_NDEBUG_INLINE Vpn_graph::Impl_::Impl_(
    ::google::protobuf::internal::InternalVisibility visibility,
    ::google::protobuf::Arena* arena)
      : _cached_size_{0} {}

inline void Vpn_graph::SharedCtor(::_pb::Arena* arena) {
  new (&_impl_) Impl_(internal_visibility(), arena);
  _impl_.id_ = {};
}
Vpn_graph::~Vpn_graph() {
  // @@protoc_insertion_point(destructor:Vpn_graph)
  _internal_metadata_.Delete<::google::protobuf::UnknownFieldSet>();
  SharedDtor();
}
inline void Vpn_graph::SharedDtor() {
  ABSL_DCHECK(GetArena() == nullptr);
  _impl_.~Impl_();
}

PROTOBUF_NOINLINE void Vpn_graph::Clear() {
// @@protoc_insertion_point(message_clear_start:Vpn_graph)
  PROTOBUF_TSAN_WRITE(&_impl_._tsan_detect_race);
  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.id_ = 0;
  _internal_metadata_.Clear<::google::protobuf::UnknownFieldSet>();
}

const char* Vpn_graph::_InternalParse(
    const char* ptr, ::_pbi::ParseContext* ctx) {
  ptr = ::_pbi::TcParser::ParseLoop(this, ptr, ctx, &_table_.header);
  return ptr;
}


PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1
const ::_pbi::TcParseTable<0, 1, 0, 0, 2> Vpn_graph::_table_ = {
  {
    0,  // no _has_bits_
    0, // no _extensions_
    1, 0,  // max_field_number, fast_idx_mask
    offsetof(decltype(_table_), field_lookup_table),
    4294967294,  // skipmap
    offsetof(decltype(_table_), field_entries),
    1,  // num_field_entries
    0,  // num_aux_entries
    offsetof(decltype(_table_), field_names),  // no aux_entries
    &_Vpn_graph_default_instance_._instance,
    ::_pbi::TcParser::GenericFallback,  // fallback
  }, {{
    // int32 id = 1;
    {::_pbi::TcParser::SingularVarintNoZag1<::uint32_t, offsetof(Vpn_graph, _impl_.id_), 63>(),
     {8, 63, 0, PROTOBUF_FIELD_OFFSET(Vpn_graph, _impl_.id_)}},
  }}, {{
    65535, 65535
  }}, {{
    // int32 id = 1;
    {PROTOBUF_FIELD_OFFSET(Vpn_graph, _impl_.id_), 0, 0,
    (0 | ::_fl::kFcSingular | ::_fl::kInt32)},
  }},
  // no aux_entries
  {{
  }},
};

::uint8_t* Vpn_graph::_InternalSerialize(
    ::uint8_t* target,
    ::google::protobuf::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Vpn_graph)
  ::uint32_t cached_has_bits = 0;
  (void)cached_has_bits;

  // int32 id = 1;
  if (this->_internal_id() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::
        WriteInt32ToArrayWithField<1>(
            stream, this->_internal_id(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target =
        ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
            _internal_metadata_.unknown_fields<::google::protobuf::UnknownFieldSet>(::google::protobuf::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Vpn_graph)
  return target;
}

::size_t Vpn_graph::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Vpn_graph)
  ::size_t total_size = 0;

  ::uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int32 id = 1;
  if (this->_internal_id() != 0) {
    total_size += ::_pbi::WireFormatLite::Int32SizePlusOne(
        this->_internal_id());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::google::protobuf::Message::ClassData Vpn_graph::_class_data_ = {
    Vpn_graph::MergeImpl,
    nullptr,  // OnDemandRegisterArenaDtor
};
const ::google::protobuf::Message::ClassData* Vpn_graph::GetClassData() const {
  return &_class_data_;
}

void Vpn_graph::MergeImpl(::google::protobuf::Message& to_msg, const ::google::protobuf::Message& from_msg) {
  auto* const _this = static_cast<Vpn_graph*>(&to_msg);
  auto& from = static_cast<const Vpn_graph&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Vpn_graph)
  ABSL_DCHECK_NE(&from, _this);
  ::uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_id() != 0) {
    _this->_internal_set_id(from._internal_id());
  }
  _this->_internal_metadata_.MergeFrom<::google::protobuf::UnknownFieldSet>(from._internal_metadata_);
}

void Vpn_graph::CopyFrom(const Vpn_graph& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Vpn_graph)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

PROTOBUF_NOINLINE bool Vpn_graph::IsInitialized() const {
  return true;
}

::_pbi::CachedSize* Vpn_graph::AccessCachedSize() const {
  return &_impl_._cached_size_;
}
void Vpn_graph::InternalSwap(Vpn_graph* PROTOBUF_RESTRICT other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
        swap(_impl_.id_, other->_impl_.id_);
}

::google::protobuf::Metadata Vpn_graph::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_vpn_5fgraph_2eproto_getter, &descriptor_table_vpn_5fgraph_2eproto_once,
      file_level_metadata_vpn_5fgraph_2eproto[0]);
}
// @@protoc_insertion_point(namespace_scope)
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"